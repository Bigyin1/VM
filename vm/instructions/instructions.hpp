/** @file */
#ifndef CPU_INSTR_META_HPP
#define CPU_INSTR_META_HPP

#include <cstddef>
#include <stdint.h>

#include <stdio.h>
#include "argument.hpp"

typedef struct InstructionMeta InstructionMeta;

typedef struct Instruction
{
    const InstructionMeta *im;
    Argument Arg1;
    Argument Arg2;
    uint8_t ArgSetIdx;

    uint8_t DataSz;
    uint8_t SignExtend;

} Instruction;

typedef enum InstrErr
{

    INSTR_UNKNOWN,
    INSTR_WRONG_OPERANDS,
    INSTR_OK,

} InstrErr;

typedef int (*DecFunc)(Instruction *, FILE *);
typedef int (*EncFunc)(Instruction *, FILE *);

typedef struct ArgSet
{
    ArgType First;
    ArgType Second;

} ArgSet;

typedef const char *InstructionName;

struct InstructionMeta
{
    InstructionName Name;
    uint8_t OpCode;
    ArgSet ArgSets[8];
    EncFunc encFunc;
    DecFunc decFunc;
};

typedef const char *RegName;

typedef struct RegMeta
{

    RegName Name;
    uint8_t RegCode;

} RegMeta;

int findRegByName(RegName name);

InstrErr NewInstruction(InstructionName name, Instruction instr, size_t *sz);

int Decode(Instruction *ins, FILE *r);

int Encode(Instruction *ins, FILE *w);

#endif
