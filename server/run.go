package main

import (
	"bytes"
	"context"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"sync"

	"github.com/gorilla/websocket"
)

type Runner struct {
	vm  string
	asm string

	resp chan<- []byte
	req  <-chan []byte

	conn *websocket.Conn

	cancel context.CancelFunc
	ctx    context.Context
}

const errorMessageType = "error"
const generalMessageType = "general"
const consOutMessageType = "console"

type textResp struct {
	MessageType string `json:"type"`
	Message     string `json:"message"`
}

type pointData struct {
	X     uint16 `json:"x"`
	Y     uint16 `json:"y"`
	Color byte   `json:"color"`
}

type consoleWriteReq struct {
	Data string `json:"val"`
}

func writeJsonToConn(conn *websocket.Conn, val interface{}) error {

	b, err := json.Marshal(val)
	if err != nil {
		return err
	}
	err = conn.WriteMessage(websocket.TextMessage, b)
	return err

}

func (r *Runner) readConn(stdinWrite *os.File) {

	defer func() {
		r.cancel()
	}()

	for {

		_, buf, err := r.conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("error: %v", err)
			}
			return
		}

		conReq := consoleWriteReq{}

		err = json.Unmarshal(buf, &conReq)
		if err != nil {
			log.Printf("readConn error: %v", err)
			return
		}

		_, err = stdinWrite.Write([]byte(conReq.Data))
		if err != nil {
			log.Printf("readConn error: %v", err)
			return
		}

		_, err = stdinWrite.Write([]byte{'\n'})
		if err != nil {
			log.Printf("readConn error: %v", err)
			return
		}

	}

}

func (r *Runner) writeConn(numConsRead, stderrRead, graphConsRead *os.File, wg *sync.WaitGroup) {

	defer func() {
		r.cancel()
		wg.Done()
	}()

	numConsBuf := make([]byte, 32)

	graphConsBuf := make([]byte, 5)

	errBuf := bytes.Buffer{}

	for {

		n, err := numConsRead.Read(numConsBuf)
		if err != nil && err != io.EOF {
			fmt.Printf("writeConn : %v\n", err)
			break
		}
		if n > 0 {
			consData := textResp{MessageType: consOutMessageType, Message: string(numConsBuf[0:n])}
			err = writeJsonToConn(r.conn, consData)
			if err != nil {
				fmt.Printf("writeConn : %v\n", err)
				return

			}
		}

		n, err = graphConsRead.Read(graphConsBuf)
		if err != nil {
			if err == io.EOF {
				break
			}
			fmt.Printf("writeConn : %v\n", err)
			break
		}
		if n != len(graphConsBuf) {
			fmt.Printf("bad majestic console: read %d bytes", n)
			break
		}

		pd := pointData{}

		pd.X = binary.LittleEndian.Uint16(graphConsBuf)
		pd.Y = binary.LittleEndian.Uint16(graphConsBuf[2:])
		pd.Color = graphConsBuf[4]

		err = writeJsonToConn(r.conn, pd)
		if err != nil {
			fmt.Printf("writeConn : %v\n", err)
			return
		}

	}

	errBuf.ReadFrom(stderrRead)

	vmErrors := textResp{MessageType: errorMessageType, Message: errBuf.String()}
	writeJsonToConn(r.conn, vmErrors)

}

func (r *Runner) monitorVM(exeFile string) {

	stdinRead, stdinWrite, err := os.Pipe()
	numConsRead, numConsWrite, err := os.Pipe()
	stderrRead, stderrWrite, err := os.Pipe()
	graphConsRead, graphConsWrite, err := os.Pipe()

	if err != nil {
		fmt.Printf("failed to open one of pipes for vm communication: %s", err)
		return
	}

	cmd := exec.Command(r.vm, exeFile)

	cmd.Stdin = stdinRead
	cmd.Stdout = numConsWrite
	cmd.Stderr = stderrWrite
	cmd.ExtraFiles = append(cmd.ExtraFiles, graphConsWrite)

	cmd.Start()

	stdinRead.Close()
	numConsWrite.Close()
	stderrWrite.Close()
	graphConsWrite.Close()

	procChan := make(chan struct{})

	go func() {
		err := cmd.Wait()
		procChan <- struct{}{}
		if err != nil {
			fmt.Printf("vm error status: %s", err.Error())
		}
	}()

	wg := sync.WaitGroup{}

	go r.readConn(stdinWrite)

	wg.Add(1)
	go r.writeConn(numConsRead, stderrRead, graphConsRead, &wg)

	select {
	case <-procChan:
		close(procChan)

	case <-r.ctx.Done():
		cmd.Process.Kill()
		cmd.Process.Release()
	}

	if r.ctx.Err() == context.DeadlineExceeded {

		timeOutMessage := textResp{MessageType: errorMessageType, Message: "Terminating: timeout"}
		writeJsonToConn(r.conn, timeOutMessage)

		r.conn.WriteMessage(websocket.CloseMessage, []byte{})

		r.conn.Close()
		return
	}

	wg.Wait()

	timeOutMessage := textResp{MessageType: generalMessageType, Message: "Completed"}
	writeJsonToConn(r.conn, timeOutMessage)

	r.conn.WriteMessage(websocket.CloseMessage, []byte{})
	r.conn.Close()

}

func (r *Runner) Start() error {

	mType, text, err := r.conn.ReadMessage()
	if err != nil {
		if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
			fmt.Printf("websoket error: %v\n", err)
		}
		return err
	}

	if mType != websocket.TextMessage {
		fmt.Printf("got uexpected websoket message type\n")
		return err
	}

	asmTextFile, err := ioutil.TempFile(".", "text")
	if err != nil {
		fmt.Printf("failed to create temp file: %s\n", err)
		return err
	}

	_, err = asmTextFile.Write(text)
	if err != nil {
		fmt.Printf("failed to write code to temp file: %s\n", err)
		return err
	}

	asmBinFile, err := ioutil.TempFile(".", "bin")
	if err != nil {
		fmt.Printf("failed to create temp file: %s\n", err)
		return err
	}
	asmTextFile.Close()
	asmBinFile.Close()

	defer func() {
		os.Remove(asmTextFile.Name())
		os.Remove(asmBinFile.Name())
	}()

	cmd := exec.Command(r.asm, asmTextFile.Name(), asmBinFile.Name())

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		fmt.Printf("asembler error: %s\n", err.(*exec.ExitError).String())

		eMes := textResp{MessageType: errorMessageType, Message: errBuf.String()}

		err = writeJsonToConn(r.conn, eMes)
		r.conn.WriteMessage(websocket.CloseMessage, []byte{})
		return err
	}

	succMes := textResp{MessageType: generalMessageType, Message: "Compiled successfully\n"}
	err = writeJsonToConn(r.conn, succMes)
	if err != nil {
		fmt.Printf("writeJsonToConn: %v", err)
		return err
	}

	r.monitorVM(asmBinFile.Name())

	return nil
}
