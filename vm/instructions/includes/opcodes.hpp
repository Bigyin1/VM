/** @file */
#ifndef CPU_INSTR_OPCODE_HPP
#define CPU_INSTR_OPCODE_HPP

typedef enum InstrOpCode
{
#define INSTR(name, opCode, ...) ins_##name = opCode,

#include "instructionsMeta.inc"

#undef INSTR

} InstrOpCode;

#endif
