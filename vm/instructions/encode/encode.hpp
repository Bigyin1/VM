/** @file */
#ifndef CPU_INS_ENCODE_HPP
#define CPU_INS_ENCODE_HPP

#include <stdio.h>

#include "instructions.hpp"

int Encode(Instruction* ins, FILE* w);

size_t EvalInstrSize(Instruction* ins);

size_t EvalInstrSymbolOffset(Instruction* ins);

#endif
