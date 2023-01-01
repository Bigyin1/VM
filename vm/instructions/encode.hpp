/** @file */
#ifndef CPU_INS_ENCODE_HPP
#define CPU_INS_ENCODE_HPP

#include "instructions.hpp"

int encodeLD(Instruction *ins, FILE *w);
int encodeST(Instruction *ins, FILE *w);
int encodeMOV(Instruction *ins, FILE *w);
int encodePUSH(Instruction *ins, FILE *w);
int encodePOP(Instruction *ins, FILE *w);
int encodeARITHM(Instruction *ins, FILE *w);
int encodeARITHMF(Instruction *ins, FILE *w);
int encodeJMP(Instruction *ins, FILE *w);

#endif
