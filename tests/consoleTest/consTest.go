package main

import (
	"bytes"
	"encoding/binary"
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

	outBuf.Reset()
	errBuf.Reset()
	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	graphicsConRead, graphicsConWrite, _ := os.Pipe()
	cmd.ExtraFiles = append(cmd.ExtraFiles, graphicsConWrite)

	err = cmd.Run()
	if err != nil {
		fmt.Println("vm error: ", err.(*exec.ExitError).String())
		fmt.Println(outBuf.String())
		fmt.Println(errBuf.String())
		os.Exit(1)
	}

	graphicsConWrite.Close()

	iters := 128
	for i := 0; i < iters; i++ {
		var x uint16
		var y uint16
		var color byte

		err := binary.Read(graphicsConRead, binary.LittleEndian, &x)
		err = binary.Read(graphicsConRead, binary.LittleEndian, &y)
		err = binary.Read(graphicsConRead, binary.LittleEndian, &color)
		if err != nil {
			fmt.Printf("met error: %s while reading graphics console", err)
			os.Exit(1)
		}

		if x != uint16(i) || y != uint16(i) || color != byte(iters-i) {
			fmt.Printf("got wrong data from graphics console: %d %d %d; wanted: %d %d %d\n",
				x, y, color, i, i, iters-i)
			os.Exit(1)
		}
	}

	buf, _ := io.ReadAll(graphicsConRead)
	if len(buf) > 0 {
		fmt.Printf("got unexpected data from graphics console")
		os.Exit(1)
	}

	defer os.Remove(binFile)

}
