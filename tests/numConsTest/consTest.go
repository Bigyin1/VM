package main

import (
	"bytes"
	"fmt"
	"io"
	"os"
	"os/exec"
)

func main() {

	if len(os.Args) != 5 {
		println("not enough args")
		os.Exit(1)
	}

	asm := os.Args[1]
	vm := os.Args[2]
	codeFile := os.Args[3]
	binFile := os.Args[4]

	defer os.Remove(binFile)

	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	cmd := exec.Command(asm, codeFile, binFile)
	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err := cmd.Run()
	if err != nil {
		fmt.Println("asm error: ", err.(*exec.ExitError).String())
		fmt.Println(outBuf.String())
		fmt.Println(errBuf.String())
		os.Exit(1)
	}

	cmd = exec.Command(vm, binFile)

	_, graphicsConWrite, _ := os.Pipe()
	stdinRead, stdinWrite, _ := os.Pipe()
	numConsRead, numConsWrite, _ := os.Pipe()
	stderrRead, stderrWrite, _ := os.Pipe()
	cmd.ExtraFiles = append(cmd.ExtraFiles, graphicsConWrite)

	cmd.Stdin = stdinRead
	cmd.Stdout = numConsWrite
	cmd.Stderr = stderrWrite

	err = cmd.Start()

	fmt.Println(cmd.Process.Pid)
	stdinRead.Close()
	numConsWrite.Close()
	stderrWrite.Close()
	graphicsConWrite.Close()

	if err != nil {
		fmt.Println("vm start error: ", err.(*exec.ExitError).String())
		os.Exit(1)
	}

	num := "12345"
	stdinWrite.Write([]byte(num))

	stdinWrite.Write([]byte("\n"))

	numBuf := make([]byte, len(num))
	numConsRead.Read(numBuf)

	if num != string(numBuf) {
		fmt.Printf("num cons test: got %s, wanted: %s\n", string(numBuf), num)
		os.Exit(1)
	}
	cmd.Wait()
	fmt.Println(cmd.ProcessState.String())
	eBuf, _ := io.ReadAll(stderrRead)
	if len(eBuf) != 0 {
		fmt.Printf("num cons test: got errors: %ss\n", string(eBuf))
		os.Exit(1)
	}

}
