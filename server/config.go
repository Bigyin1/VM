package main

import "flag"

var stdlib = []string{
	"stdlib/bin/linear.o",
	"stdlib/bin/printStr.o",
	"stdlib/bin/quad.o",
	"stdlib/bin/quadEqSolver.o",
	"stdlib/bin/drawCircle.o",
}

var addr = flag.String("addr", ":8080", "http service address")
var asmPath = flag.String("asm", "./progs/my_asm", "assermbler path") // TODO: make "better" configuration
var vmPath = flag.String("vm", "./progs/my_vm", "vm path")
var ldPath = flag.String("ld", "./progs/my_ld", "ld path")
var readobjPath = flag.String("readobj", "./progs/readobj", "readobj path")
