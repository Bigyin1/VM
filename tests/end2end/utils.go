package main

import (
	"bytes"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"syscall"
	"testing"
	"time"
)

const asmExePath = "progs/my_asm"
const vmExePath = "progs/my_vm"

func compileFile(t *testing.T, exePath string) *os.File {

	binFile, err := ioutil.TempFile(".", "bin")
	if err != nil {
		t.Errorf("failed to create temp file: %s\n", err)
		return nil
	}

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd := exec.Command(asmExePath, exePath, binFile.Name())
	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		t.Errorf("%s\n", errBuf.String())
		t.Errorf("asm error: %s\n", err.(*exec.ExitError).String())
		return nil
	}

	return binFile

}

type testVM struct {
	cmd *exec.Cmd

	errBuf bytes.Buffer

	graphics io.ReadCloser

	vmConsoleWriter io.WriteCloser
	vmConsoleReader io.ReadCloser

	t *testing.T
}

func (vm *testVM) watchVM(t time.Duration) {

	<-time.After(t)

	vm.cmd.Process.Signal(syscall.SIGTERM)
	vm.cmd.Process.Release()

}

func (vm *testVM) startVM(binFile string) {

	cmd := exec.Command(vmExePath, binFile)
	vm.cmd = cmd

	vmConsoleReaderTheir, vmConsoleWriterOur, err := os.Pipe()
	vmConsoleReaderOur, vmConsoleWriterTheir, err := os.Pipe()
	graphicsReadOur, graphicsWriteTheir, err := os.Pipe()
	if err != nil {
		vm.t.Fatal(err)
		return
	}

	cmd.Stdin = vmConsoleReaderTheir
	cmd.Stdout = vmConsoleWriterTheir
	cmd.Stderr = &vm.errBuf

	vm.graphics = graphicsReadOur
	vm.vmConsoleWriter = vmConsoleWriterOur
	vm.vmConsoleReader = vmConsoleReaderOur

	cmd.ExtraFiles = append(cmd.ExtraFiles, graphicsWriteTheir)

	err = cmd.Start()
	if err != nil {
		vm.t.Fatal(err)
		return
	}

	graphicsWriteTheir.Close()
	vmConsoleReaderTheir.Close()
	vmConsoleWriterTheir.Close()

}

func (vm *testVM) waitVM() {

	vm.cmd.Wait()
	if !vm.cmd.ProcessState.Success() {
		vm.t.Errorf("%s\n", vm.errBuf.String())
		vm.t.Errorf("vm failed: %s", vm.cmd.ProcessState.String())
	}
}
