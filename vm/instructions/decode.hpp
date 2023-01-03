/** @file */
#ifndef CPU_INS_DECODE_HPP
#define CPU_INS_DECODE_HPP

#include "instructions.hpp"

InstrErr decodeLD(Instruction *ins, FILE *r);
InstrErr decodeST(Instruction *ins, FILE *r);
InstrErr decodeMOV(Instruction *ins, FILE *r);
InstrErr decodePUSH(Instruction *ins, FILE *r);
InstrErr decodePOP(Instruction *ins, FILE *r);
InstrErr decodeARITHM(Instruction *ins, FILE *r);
InstrErr decodeARITHMF(Instruction *ins, FILE *r);
InstrErr decodeBranch(Instruction *ins, FILE *r);
InstrErr decodeNoArgs(Instruction *ins, FILE *r);

#endif
