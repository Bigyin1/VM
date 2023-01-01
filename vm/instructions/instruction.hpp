/** @file */
#ifndef CPU_INSTRUCTIONS_HPP
#define CPU_INSTRUCTIONS_HPP

#include <cstddef>
#include <stdint.h>
#include "instructionsMeta.hpp"


typedef struct Instruction
{
    const InstructionMeta   *im;
    Argument                Arg1;
    Argument                Arg2;
    uint8_t                 ArgSetIdx;

    uint8_t                 DataSz;
    uint8_t                 SignExtend;

} Instruction;


typedef enum InstrErr {

    INSTR_UNKNOWN,
    INSTR_WRONG_OPERANDS,
    INSTR_OK,

} InstrErr;




#endif
