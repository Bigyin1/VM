package main

import (
	"bytes"
	"fmt"
	"math"
	"os"
	"testing"
)

func floatsAlmostEqual(a, b float64) bool {
	return math.Abs(a-b) <= 1e-9
}

const quadEqNoSolutions = 1
const quadEqInfSolutions = 2
const quadEqOneSolution = 3
const quadEqTwoSolutions = 4

type quadEqTestCase struct {
	a float64
	b float64
	c float64

	root1    float64
	root2    float64
	solState byte
}

var testCases = []quadEqTestCase{

	{a: 1.0, b: 2.0, c: 1.0, root1: -1.0, root2: -1.0, solState: quadEqTwoSolutions},
	{a: 0, b: 2.0, c: 1.0, root1: -0.5, root2: 0, solState: quadEqOneSolution},
	{a: 0, b: 0, c: 0, root1: 0, root2: 0, solState: quadEqInfSolutions},
	{a: 1, b: 3, c: 5, root1: 0, root2: 0, solState: quadEqNoSolutions},
	{a: 0, b: 0, c: -9, root1: 0, root2: 0, solState: quadEqNoSolutions},
}

func TestQuadEq(t *testing.T) {

	asmSrcs := []string{
		"linear.code",
		"quad.code",
		"quadEqSolver.code",
	}

	for i := range asmSrcs {
		asmSrcs[i] = "testdata/quadEq/" + asmSrcs[i]
	}

	binFile := compileAndLinkFiles(t, asmSrcs)
	if t.Failed() {
		return
	}

	defer os.Remove(binFile)

	vm := testVM{t: t}

	for _, v := range testCases {

		vm.startVM(binFile)
		if t.Failed() {
			return
		}

		arg1 := fmt.Sprint(v.a) + "\n"
		_, err := vm.vmConsoleWriter.Write([]byte(arg1))
		if err != nil {
			vm.t.Errorf("pipe write error: %s", err)
			return
		}

		_, err = vm.vmConsoleWriter.Write([]byte(fmt.Sprint(v.b) + "\n"))
		if err != nil {
			vm.t.Errorf("pipe write error: %s", err)
			return
		}

		_, err = vm.vmConsoleWriter.Write([]byte(fmt.Sprint(v.c) + "\n"))
		if err != nil {
			vm.t.Errorf("pipe write error: %s", err)
			return
		}

		var buf bytes.Buffer
		_, err = buf.ReadFrom(vm.vmConsoleReader)
		if err != nil {
			vm.t.Errorf("pipe read error: %s", err)
			vm.t.Error(vm.errBuf.String())
			return
		}
		var root1, root2 float64
		var solState byte
		fmt.Fscan(&buf, &root1, &root2, &solState)

		vm.waitVM()
		if t.Failed() {
			return
		}

		if !floatsAlmostEqual(root1, v.root1) ||
			!floatsAlmostEqual(root2, v.root2) ||
			solState != v.solState {

			t.Errorf("unexpected output: %f %f %d\n", root1, root2, solState)
			t.Errorf("wanted: %f %f %d\n", v.root1, v.root2, v.solState)
			return
		}

	}

}
