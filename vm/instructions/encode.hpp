/** @file */
#ifndef CPU_INS_ENCODE_HPP
#define CPU_INS_ENCODE_HPP

#include "instructions.hpp"

size_t encodeLD(Instruction *ins, FILE *w, bool);
size_t encodeST(Instruction *ins, FILE *w, bool);
size_t encodeMOV(Instruction *ins, FILE *w, bool);
size_t encodePUSH(Instruction *ins, FILE *w, bool);
size_t encodePOP(Instruction *ins, FILE *w, bool);
size_t encodeARITHM(Instruction *ins, FILE *w, bool);
size_t encodeARITHMF(Instruction *ins, FILE *w, bool);
size_t encodeJMP(Instruction *ins, FILE *w, bool);
size_t encodeCALL(Instruction *ins, FILE *w, bool);
size_t encodeNoArgs(Instruction *ins, FILE *w, bool);

#endif
