/** @file */
#ifndef VM_HPP
#define VM_HPP

#include <stddef.h>
#include "cpu.hpp"

int InitVM(CPU *cpu, FILE *prog);

void DestructVM(CPU *cpu);

void RunVM(CPU *cpu);

#endif
