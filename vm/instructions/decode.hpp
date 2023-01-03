/** @file */
#ifndef CPU_INS_DECODE_HPP
#define CPU_INS_DECODE_HPP

#include "instructions.hpp"

InstrDecErr decodeLD(Instruction *ins, FILE *r);
InstrDecErr decodeST(Instruction *ins, FILE *r);
InstrDecErr decodeMOV(Instruction *ins, FILE *r);
InstrDecErr decodePUSH(Instruction *ins, FILE *r);
InstrDecErr decodePOP(Instruction *ins, FILE *r);
InstrDecErr decodeARITHM(Instruction *ins, FILE *r);
InstrDecErr decodeARITHMF(Instruction *ins, FILE *r);
InstrDecErr decodeBranch(Instruction *ins, FILE *r);
InstrDecErr decodeNoArgs(Instruction *ins, FILE *r);

#endif
