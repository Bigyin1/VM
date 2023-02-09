/** @file */
#ifndef VM_HPP
#define VM_HPP

#include <stdbool.h>
#include <stddef.h>

#include "cpu.hpp"

typedef struct VMConfig
{
    bool attachConsole;
    bool attachRAM;
    bool attachROM;

} VMConfig;

int InitVM(CPU* cpu, const VMConfig* cfg);

int LoadExeFile(CPU* cpu, FILE* in);

void RunVM(CPU* cpu);

void DestructVM(CPU* cpu, const VMConfig* cfg);

extern const size_t romDevIdx;
extern const size_t ramDevIdx;
extern const size_t consoleDevIdx;

#endif
