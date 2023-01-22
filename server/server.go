package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
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

	cmd := exec.Command(asm, codeFile, binFile)

	var outBuf bytes.Buffer
	cmd.Stdout = &outBuf

	var errBuf bytes.Buffer
	cmd.Stderr = &errBuf

	err := cmd.Run()
	if err != nil {
		fmt.Println("Exec error: ", err)
		fmt.Println(outBuf.String())
		fmt.Println(errBuf.String())
		return
	}

	cmd = exec.Command(vm, binFile)

	grPipe, w, _ := os.Pipe()

	cmd.ExtraFiles = append(cmd.ExtraFiles, w)

	outBuf.Reset()
	errBuf.Reset()
	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err = cmd.Run()
	if err != nil {
		fmt.Println("Exec error: ", err)
		fmt.Println(outBuf.String())
		fmt.Println(errBuf.String())
		return
	}

	fmt.Println(outBuf.String())
	fmt.Println(errBuf.String())

	w.Close()
	//buf, _ := ioutil.ReadAll(grPipe)

	for i := 0; i < 128; i++ {
		var x uint16
		var y uint16
		var color byte
		err := binary.Read(grPipe, binary.LittleEndian, &x)
		fmt.Println(err)
		binary.Read(grPipe, binary.LittleEndian, &y)
		binary.Read(grPipe, binary.LittleEndian, &color)
		fmt.Printf("%d %d %d\n", x, y, color)
	}
	//fmt.Println(string(buf))

}
