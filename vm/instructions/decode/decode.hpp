/** @file */
#ifndef CPU_INS_DECODE_HPP
#define CPU_INS_DECODE_HPP

#include "instructions.hpp"

InstrCreationErr Decode(Instruction *ins, FILE *r);

#endif
