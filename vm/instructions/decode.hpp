/** @file */
#ifndef CPU_INS_DECODE_HPP
#define CPU_INS_DECODE_HPP

#include "instructions.hpp"

int decodeLD(Instruction *ins, FILE *r);
int decodeST(Instruction *ins, FILE *r);
int decodeMOV(Instruction *ins, FILE *r);
int decodePUSH(Instruction *ins, FILE *r);
int decodePOP(Instruction *ins, FILE *r);
int decodeARITHM(Instruction *ins, FILE *r);
int decodeARITHMF(Instruction *ins, FILE *r);
int decodeJMP(Instruction *ins, FILE *r);

#endif
