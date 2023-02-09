/** @file */
#ifndef CPU_INS_DECODE_HPP
#define CPU_INS_DECODE_HPP

#include <stdio.h>

#include "instructions.hpp"

InstrCreationErr Decode(Instruction* ins, FILE* r);

#endif
