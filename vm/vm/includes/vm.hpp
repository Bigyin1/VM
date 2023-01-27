/** @file */
#ifndef VM_HPP
#define VM_HPP

#include <stddef.h>
#include "cpu.hpp"

int InitVM(CPU *cpu);

int LoadExeFile(CPU *cpu, FILE *in);

void RunVM(CPU *cpu);

void DestructVM(CPU *cpu);

extern const size_t romDevIdx;
extern const size_t ramDevIdx;
extern const size_t consoleDevIdx;

#endif
