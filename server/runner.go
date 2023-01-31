package main

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"sync"
	"syscall"
)

type Runner struct {
	vmExePath  string
	asmExePath string
	ldExePath  string

	textResponse   chan<- []byte
	binaryResponse chan<- []byte
	request        <-chan []byte

	vmProc *exec.Cmd

	cancel context.CancelFunc
	ctx    context.Context

	vmConsoleReader io.ReadCloser
	vmConsoleWriter io.WriteCloser

	vmGraphicsReader io.ReadCloser

	vmErrorsReader io.ReadCloser
}

func (r *Runner) responseJson(val interface{}) error {

	b, err := json.Marshal(val)
	if err != nil {
		return err
	}

	select {
	case r.textResponse <- b:
	case <-r.ctx.Done():
	}

	return nil
}

func (r *Runner) responseBinary(data []byte) {

	select {
	case r.binaryResponse <- data:
	case <-r.ctx.Done():
	}

}

func (r *Runner) setupVM(exeFile string) error {

	vmConsoleReaderOur, vmConsoleWriterTheir, err := os.Pipe()
	vmConsoleReaderTheir, vmConsoleWriterOur, err := os.Pipe()
	vmGraphicsReaderOur, vmGraphicsWriterTheir, err := os.Pipe()
	vmErrorsReaderOur, vmErrorsWriterTheir, err := os.Pipe()
	if err != nil {
		//TODO close pipes
		fmt.Printf("failed to open one of pipes for vm communication: %s\n", err)
		return err
	}

	r.vmProc = exec.Command(r.vmExePath, exeFile)

	r.vmProc.Stdin = vmConsoleReaderTheir
	r.vmProc.Stdout = vmConsoleWriterTheir
	r.vmProc.Stderr = vmErrorsWriterTheir
	r.vmProc.ExtraFiles = append(r.vmProc.ExtraFiles, vmGraphicsWriterTheir)

	r.vmConsoleReader = vmConsoleReaderOur
	r.vmErrorsReader = vmErrorsReaderOur
	r.vmConsoleWriter = vmConsoleWriterOur
	r.vmGraphicsReader = vmGraphicsReaderOur

	r.vmProc.Start()

	vmConsoleReaderTheir.Close()
	vmConsoleWriterTheir.Close()
	vmGraphicsWriterTheir.Close()
	vmErrorsWriterTheir.Close()

	return nil
}

func (r *Runner) termSubprocess(procChan <-chan struct{}) {

	<-r.ctx.Done()

	select {
	case <-procChan:
		return
	default:
	}

	r.vmProc.Process.Signal(syscall.SIGTERM)
	r.vmProc.Wait()
	log.Printf("process %d terminated", r.vmProc.ProcessState.Pid())

}

func (r *Runner) monitorVM() error {

	procChan := make(chan struct{}, 1)
	go func() {
		r.vmProc.Wait()

		procChan <- struct{}{}

		log.Printf("process %d exited; status: %s",
			r.vmProc.ProcessState.Pid(), r.vmProc.ProcessState.String())
	}()

	wg := &sync.WaitGroup{}
	go r.inputMonitor()

	wg.Add(3)
	go r.monitorConsoleOut(wg)
	go r.monitorGraphicsOut(wg)
	go r.monitorErrorsOut(wg)

	go r.termSubprocess(procChan)

	wg.Wait()

	log.Printf("monitorVM done")

	return nil

}

const compileSuccessMessage = "Compiled successfully\n"
const linkedSuccessMessage = "Linked successfully\n"

func (r *Runner) compile() (*os.File, error) {

	code, ok := <-r.request
	if !ok {
		return nil, fmt.Errorf("no data to compile")
	}

	asmTextFile, err := ioutil.TempFile(".", "text")
	if err != nil {
		log.Printf("failed to create temp file: %s\n", err)
		return nil, err
	}
	defer os.Remove(asmTextFile.Name())

	_, err = asmTextFile.Write(code)
	if err != nil {
		log.Printf("failed to write code to temp file: %s\n", err)
		return nil, err
	}

	asmLinkableFile, err := ioutil.TempFile(".", "bin")
	if err != nil {
		log.Printf("failed to create temp file: %s\n", err)
		return nil, err
	}

	asmTextFile.Close()
	asmLinkableFile.Close()

	cmd := exec.Command(r.asmExePath, asmTextFile.Name(), asmLinkableFile.Name())

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		log.Printf("error while assembling. exit code: %s\n", err.(*exec.ExitError).String())

		os.Remove(asmLinkableFile.Name())

		errMes := textResp{
			MessageType: errorMessageType,
			Message:     errBuf.String(),
		}
		r.responseJson(errMes)
		return nil, err
	}

	succMes := textResp{
		MessageType: generalMessageType,
		Message:     compileSuccessMessage,
	}

	err = r.responseJson(succMes)
	if err != nil {

		os.Remove(asmLinkableFile.Name())
		fmt.Printf("writeJsonToConn: %v", err)
		return nil, err
	}

	return asmLinkableFile, nil
}

func (r *Runner) link(linkableFile *os.File) (*os.File, error) {

	defer os.Remove(linkableFile.Name())

	asmExeFile, err := ioutil.TempFile(".", "bin")
	if err != nil {
		log.Printf("failed to create temp file: %s\n", err)
		return nil, err
	}

	asmExeFile.Close()

	cmd := exec.Command(r.ldExePath,
		"--text=0", "--data=6000", linkableFile.Name(), asmExeFile.Name())

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		log.Printf("error while linking. exit code: %s\n", err.(*exec.ExitError).String())

		os.Remove(asmExeFile.Name())

		errMes := textResp{
			MessageType: errorMessageType,
			Message:     errBuf.String(),
		}
		r.responseJson(errMes)
		return nil, err
	}

	succMes := textResp{
		MessageType: generalMessageType,
		Message:     linkedSuccessMessage,
	}

	err = r.responseJson(succMes)
	if err != nil {
		os.Remove(asmExeFile.Name())

		fmt.Printf("responseJson: %v", err)
		return nil, err
	}

	return asmExeFile, nil

}

func (r *Runner) Start() error {

	defer func() {
		close(r.textResponse)
		close(r.binaryResponse)
	}()

	LinkFile, err := r.compile()
	if err != nil {
		// write error to user
		return err
	}

	ExeFile, err := r.link(LinkFile)
	if err != nil {
		// write error to user
		return err
	}

	defer os.Remove(ExeFile.Name())

	err = r.setupVM(ExeFile.Name())
	if err != nil {
		return err
	}

	return r.monitorVM()
}
