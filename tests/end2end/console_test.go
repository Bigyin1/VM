package main

import (
	"os"
	"strings"
	"testing"
)

func TestConsole(t *testing.T) {

	binFile := compileAndLinkFiles(t, []string{"testdata/consTest.code"})
	if t.Failed() {
		return
	}

	defer os.Remove(binFile)

	vm := testVM{t: t}

	dataInt := "12345\n"
	dataDouble := "3.134345\n"
	dataChar := "A"

	vm.startVM(binFile)
	if t.Failed() {
		return
	}

	defer vm.graphics.Close()

	consoleCheck(&vm, []byte(dataInt), []byte(strings.TrimSuffix(dataInt, "\n")))

	consoleCheck(&vm, []byte(dataDouble), []byte(strings.TrimSuffix(dataDouble, "\n")))

	consoleCheck(&vm, []byte(dataChar), []byte(dataChar))

	vm.waitVM()

}
