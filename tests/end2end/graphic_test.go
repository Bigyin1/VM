package main

import (
	"encoding/binary"
	"io"
	"os"
	"testing"
)

func TestGraphic(t *testing.T) {

	binFile := compileFile(t, "testdata/graphicTest.code")
	if t.Failed() {
		return
	}
	defer os.Remove(binFile.Name())

	vm := testVM{t: t}

	vm.startVM(binFile.Name())
	if t.Failed() {
		return
	}

	defer vm.graphics.Close()

	iters := 512
	for i := 0; i < iters; i++ {
		var x uint16
		var y uint16
		var color byte

		graphicsBuf := make([]byte, 5)

		n, err := vm.graphics.Read(graphicsBuf)
		if err != nil {
			vm.t.Fatal(err)
			return
		}

		if n != len(graphicsBuf) {
			vm.t.Fatalf("monitorGraphicsOut error: got invalid point data of size: %d", n)
			return
		}

		x = binary.LittleEndian.Uint16(graphicsBuf)
		y = binary.LittleEndian.Uint16(graphicsBuf[2:])
		color = graphicsBuf[4]

		if x != uint16(i) || y != uint16(i) || color != byte(iters-i) {
			vm.t.Errorf("got wrong data from graphics console: %d %d %d; wanted: %d %d %d\n",
				x, y, color, i, i, iters-i)
			return
		}
	}

	buf, _ := io.ReadAll(vm.graphics)
	if len(buf) > 0 {
		vm.t.Error("got unexpected data from graphics console")
		return
	}

	vm.waitVM()

}
