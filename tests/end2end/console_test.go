package main

import (
	"bytes"
	"os"
	"strings"
	"testing"
)

func consoleCheck(vm *testVM, dataIn []byte, dataExp []byte) {

	_, err := vm.vmConsoleWriter.Write(dataIn)
	if err != nil {
		vm.t.Errorf("pipe write error: %s", err)
		return
	}

	buf := make([]byte, len(dataExp))
	_, err = vm.vmConsoleReader.Read(buf)
	if err != nil {
		vm.t.Errorf("pipe read error: %s", err)
		return
	}

	if bytes.Compare(buf, dataExp) != 0 {
		vm.t.Errorf("got: %s wanted: %s", string(buf), string(dataExp))
		return
	}

}

func TestConsole(t *testing.T) {

	binFile := compileFile(t, "testdata/consTest.code")
	if t.Failed() {
		return
	}

	defer os.Remove(binFile.Name())

	vm := testVM{t: t}

	dataInt := "12345\n"
	dataDouble := "3.134345\n"
	dataChar := "A"

	vm.startVM(binFile.Name())
	if t.Failed() {
		return
	}

	defer vm.graphics.Close()

	consoleCheck(&vm, []byte(dataInt), []byte(strings.TrimSuffix(dataInt, "\n")))

	consoleCheck(&vm, []byte(dataDouble), []byte(strings.TrimSuffix(dataDouble, "\n")))

	consoleCheck(&vm, []byte(dataChar), []byte(dataChar))

	vm.waitVM()

}
