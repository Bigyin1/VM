/** @file */
#ifndef CPU_INS_ENCODE_HPP
#define CPU_INS_ENCODE_HPP

#include "instructions.hpp"

int encodeLD(Instruction *ins, FILE *w, bool);
int encodeST(Instruction *ins, FILE *w, bool);
int encodeMOV(Instruction *ins, FILE *w, bool);
int encodePUSH(Instruction *ins, FILE *w, bool);
int encodePOP(Instruction *ins, FILE *w, bool);
int encodeARITHM(Instruction *ins, FILE *w, bool);
int encodeARITHMF(Instruction *ins, FILE *w, bool);
int encodeJMP(Instruction *ins, FILE *w, bool);
int encodeNoArgs(Instruction *ins, FILE *w, bool);

#endif
