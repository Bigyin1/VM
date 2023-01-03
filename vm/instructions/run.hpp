/** @file */
#ifndef CPU_INS_RUN_HPP
#define CPU_INS_RUN_HPP

#include "instructions.hpp"
#include "../vm.hpp"

int runRET(CPU *cpu, Instruction *ins);
int runLD(CPU *cpu, Instruction *ins);
int runST(CPU *cpu, Instruction *ins);
int runMOV(CPU *cpu, Instruction *ins);
int runPUSH(CPU *cpu, Instruction *ins);
int runPOP(CPU *cpu, Instruction *ins);
int runADD(CPU *cpu, Instruction *ins);
int runADDF(CPU *cpu, Instruction *ins);
int runSUB(CPU *cpu, Instruction *ins);
int runSUBF(CPU *cpu, Instruction *ins);
int runMUL(CPU *cpu, Instruction *ins);
int runMULF(CPU *cpu, Instruction *ins);
int runDIV(CPU *cpu, Instruction *ins);
int runDIVF(CPU *cpu, Instruction *ins);
int runJMP(CPU *cpu, Instruction *ins);
int runJEQ(CPU *cpu, Instruction *ins);
int runJNEQ(CPU *cpu, Instruction *ins);
int runJG(CPU *cpu, Instruction *ins);
int runJL(CPU *cpu, Instruction *ins);
int runCALL(CPU *cpu, Instruction *ins);
int runCMP(CPU *cpu, Instruction *ins);

#endif
