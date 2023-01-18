/** @file */
#ifndef CPU_INS_ENCODE_HPP
#define CPU_INS_ENCODE_HPP

#include "instructions.hpp"

int Encode(Instruction *ins, FILE *w);

size_t EvalInstrSize(Instruction *ins);

#endif