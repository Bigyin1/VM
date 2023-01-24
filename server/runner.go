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

	response chan<- []byte
	request  <-chan []byte

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
	case r.response <- b:
	case <-r.ctx.Done():
	}

	return nil
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
	r.vmProc.Process.Release()
	log.Printf("process %d terminated", r.vmProc.Process.Pid)

}

func (r *Runner) monitorVM() error {

	procChan := make(chan struct{}, 1)
	go func() {
		r.vmProc.Wait()

		procChan <- struct{}{}

		log.Printf("process %d exited; status: %s",
			r.vmProc.Process.Pid, r.vmProc.ProcessState.String())
	}()

	wg := &sync.WaitGroup{}
	go r.readMonitor()

	wg.Add(3)
	go r.monitorConsoleOut(wg)
	go r.monitorGraphicsOut(wg)
	go r.monitorErrorsOut(wg)

	go r.termSubprocess(procChan)

	wg.Wait()

	log.Printf("monitorVM done")

	return nil

}

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

	_, err = asmTextFile.Write(code)
	if err != nil {
		log.Printf("failed to write code to temp file: %s\n", err)
		return nil, err
	}

	asmBinFile, err := ioutil.TempFile(".", "bin")
	if err != nil {
		log.Printf("failed to create temp file: %s\n", err)
		return nil, err
	}

	defer os.Remove(asmTextFile.Name())

	asmTextFile.Close()
	asmBinFile.Close()

	cmd := exec.Command(r.asmExePath, asmTextFile.Name(), asmBinFile.Name())

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		log.Printf("asembler error: %s\n", err.(*exec.ExitError).String())

		errMes := textResp{
			MessageType: errorMessageType,
			Message:     errBuf.String(),
		}
		r.responseJson(errMes)
		return nil, err
	}

	succMes := textResp{
		MessageType: generalMessageType,
		Message:     "Compiled successfully\n",
	}

	err = r.responseJson(succMes)
	if err != nil {
		fmt.Printf("writeJsonToConn: %v", err)
		return nil, err
	}

	return asmBinFile, nil
}

func (r *Runner) Start() error {

	defer close(r.response)

	BinFile, err := r.compile()
	if err != nil {
		// write error to user
		return err
	}
	defer os.Remove(BinFile.Name())

	err = r.setupVM(BinFile.Name())
	if err != nil {
		return err
	}

	return r.monitorVM()
}
