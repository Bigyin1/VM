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
const ldExePath = "progs/my_ld"
const vmExePath = "progs/my_vm"

func compileFile(t *testing.T, path string) string {

	linkableFile, err := ioutil.TempFile(".", "link")
	if err != nil {
		t.Errorf("failed to create temp file: %s\n", err)
		return ""
	}
	linkableFile.Close()

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd := exec.Command(asmExePath, path, linkableFile.Name())
	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		os.Remove(linkableFile.Name())
		t.Errorf("%s\n", errBuf.String())
		return ""
	}

	return linkableFile.Name()

}

func compileAndLinkFiles(t *testing.T, asmPaths []string) string {

	linkedPaths := make([]string, 0)

	for _, asmPath := range asmPaths {
		linkedPath := compileFile(t, asmPath)
		if t.Failed() {
			return ""
		}
		defer os.Remove(linkedPath)
		linkedPaths = append(linkedPaths, linkedPath)
	}

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	exeFile, err := ioutil.TempFile(".", "exe")
	if err != nil {
		t.Errorf("failed to create temp file: %s\n", err)
		return ""
	}
	exeFile.Close()

	cmdArgs := append(linkedPaths, "--text=0", exeFile.Name())

	cmd := exec.Command(ldExePath, cmdArgs...)
	err = cmd.Run()
	if err != nil {
		os.Remove(exeFile.Name())
		t.Errorf("%s\n", errBuf.String())
		t.Errorf("%s\n", outBuf.String())
		return ""
	}

	return exeFile.Name()

}

type testVM struct {
	cmd *exec.Cmd

	errBuf bytes.Buffer

	graphics io.ReadCloser

	vmConsoleWriter io.WriteCloser
	vmConsoleReader io.ReadCloser

	t *testing.T
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
		vm.t.Error(err)
		return
	}

	graphicsWriteTheir.Close()
	vmConsoleReaderTheir.Close()
	vmConsoleWriterTheir.Close()

}

func consoleEchoCheck(vm *testVM, dataIn []byte, dataExp []byte) {

	_, err := vm.vmConsoleWriter.Write(dataIn)
	if err != nil {
		vm.t.Errorf("pipe write error: %s", err)
		return
	}

	buf := make([]byte, len(dataExp))
	_, err = vm.vmConsoleReader.Read(buf)
	if err != nil {
		vm.t.Errorf("pipe read error: %s", err)
		vm.t.Error(vm.errBuf.String())
		return
	}

	if !bytes.Equal(buf, dataExp) {
		vm.t.Errorf("got: %s wanted: %s", string(buf), string(dataExp))
		return
	}

}

func (vm *testVM) waitVM() {

	vm.cmd.Wait()
	if !vm.cmd.ProcessState.Success() {
		vm.t.Errorf("%s\n", vm.errBuf.String())
		vm.t.Errorf("vm failed: %s", vm.cmd.ProcessState.String())
	}

	vm.vmConsoleReader.Close()
	vm.vmConsoleWriter.Close()
	vm.graphics.Close()
}

func (vm *testVM) watchVM(t time.Duration) {

	<-time.After(t)

	vm.cmd.Process.Signal(syscall.SIGTERM)
	vm.cmd.Process.Release()

}
