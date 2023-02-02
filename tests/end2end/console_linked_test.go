package main

import (
	"os"
	"strings"
	"testing"
)

func TestLinkedConsole(t *testing.T) {

	asmSrcs := []string{
		"ConsAddrs.code",
		"RWconsChar.code",
		"RWconsDouble.code",
		"main.code",
		"RWconsInt.code",
	}

	for i := range asmSrcs {
		asmSrcs[i] = "testdata/consTestLinked/" + asmSrcs[i]
	}

	binFile := compileAndLinkFiles(t, asmSrcs)
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

	consoleEchoCheck(&vm, []byte(dataInt), []byte(strings.TrimSuffix(dataInt, "\n")))

	consoleEchoCheck(&vm, []byte(dataDouble), []byte(strings.TrimSuffix(dataDouble, "\n")))

	consoleEchoCheck(&vm, []byte(dataChar), []byte(dataChar))

	vm.waitVM()

}
