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
)

type Runner struct {
	vmExePath      string
	asmExePath     string
	ldExePath      string
	readobjExePath string

	textResponse   chan<- []byte
	binaryResponse chan<- []byte
	request        <-chan []byte

	vmProc *exec.Cmd

	ctx context.Context

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

	vmConsoleReaderOur, vmConsoleWriterTheir, _ := os.Pipe()
	vmConsoleReaderTheir, vmConsoleWriterOur, _ := os.Pipe()
	vmGraphicsReaderOur, vmGraphicsWriterTheir, _ := os.Pipe()
	vmErrorsReaderOur, vmErrorsWriterTheir, _ := os.Pipe()

	defer func() {
		vmConsoleReaderTheir.Close()
		vmConsoleWriterTheir.Close()
		vmGraphicsWriterTheir.Close()
		vmErrorsWriterTheir.Close()
	}()

	r.vmProc = exec.CommandContext(r.ctx, r.vmExePath, exeFile)

	r.vmProc.Stdin = vmConsoleReaderTheir
	r.vmProc.Stdout = vmConsoleWriterTheir
	r.vmProc.Stderr = vmErrorsWriterTheir
	r.vmProc.ExtraFiles = append(r.vmProc.ExtraFiles, vmGraphicsWriterTheir)

	r.vmConsoleReader = vmConsoleReaderOur
	r.vmErrorsReader = vmErrorsReaderOur
	r.vmConsoleWriter = vmConsoleWriterOur
	r.vmGraphicsReader = vmGraphicsReaderOur

	return r.vmProc.Start()
}

func (r *Runner) monitorVM() error {

	wg := &sync.WaitGroup{}
	wg.Add(4)

	go func() {

		defer wg.Done()

		err := r.vmProc.Wait()
		if err != nil {
			log.Printf("process %d failed: %s", r.vmProc.Process.Pid, err)
			return
		}

		log.Printf("process %d exited; status: %s",
			r.vmProc.ProcessState.Pid(), r.vmProc.ProcessState.String())
	}()

	go r.inputMonitor()

	go r.monitorConsoleOut(wg)
	go r.monitorGraphicsOut(wg)
	go r.monitorErrorsOut(wg)

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

	cmd := exec.CommandContext(r.ctx, r.asmExePath,
		asmTextFile.Name(), asmLinkableFile.Name())

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		log.Printf("error while assembling. exit code: %s\n", err)

		os.Remove(asmLinkableFile.Name())

		errMes := textResp{
			MessageType: errorMessageType,
			Message:     errBuf.String(),
		}
		_ = r.responseJson(errMes)
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

	asmExeFile, err := os.CreateTemp(".", "bin")
	if err != nil {
		log.Printf("failed to create temp file: %s\n", err)
		return nil, err
	}

	asmExeFile.Close()

	ldArgs := append(stdlib, linkableFile.Name(), "--text=0", "--data=5500", asmExeFile.Name())

	cmd := exec.CommandContext(r.ctx, r.ldExePath, ldArgs...)

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
		_ = r.responseJson(errMes)
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

func (r *Runner) readobjDump(exeFile string) error {

	cmd := exec.CommandContext(r.ctx, r.readobjExePath, exeFile)

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err := cmd.Run()
	if err != nil {
		log.Printf("readobj failed: exit code: %s\n", err.(*exec.ExitError).String())

		errMes := textResp{
			MessageType: errorMessageType,
			Message:     errBuf.String(),
		}
		_ = r.responseJson(errMes)
		return err
	}

	succMes := textResp{
		MessageType: codeDumpMessageType,
		Message:     outBuf.String(),
	}

	err = r.responseJson(succMes)
	if err != nil {
		fmt.Printf("responseJson: %v", err)
		return err
	}

	return nil
}

func (r *Runner) Start() error {

	defer func() {
		close(r.textResponse)
		close(r.binaryResponse)
	}()

	linkableFile, err := r.compile()
	if err != nil {
		// log error
		return err
	}

	exeFile, err := r.link(linkableFile)
	if err != nil {
		// log error
		return err
	}
	defer os.Remove(exeFile.Name())

	if err = r.readobjDump(exeFile.Name()); err != nil {
		return err
	}

	err = r.setupVM(exeFile.Name())
	if err != nil {
		return err
	}

	return r.monitorVM()
}
