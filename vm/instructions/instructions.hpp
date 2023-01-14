/** @file */
#ifndef CPU_INSTRUCTS_HPP
#define CPU_INSTRUCTS_HPP

#include <cstddef>
#include <stdint.h>

#include <stdio.h>
#include "argument.hpp"
#include "opcodes.hpp"
#include "../vm.hpp"

typedef enum JumpType
{

    JumpUncond = 0, // default value
    JumpEQ,         // ==
    JumpNEQ,        // !=
    JumpG,          // >
    JumpGE,         // >=
    JumpL,          // <
    JumpLE,         // <=

} JumpType;

typedef struct InstructionMeta InstructionMeta;

typedef struct Instruction
{
    const InstructionMeta *im;
    Argument Arg1;
    Argument Arg2;
    uint8_t ArgSetIdx;

    DataSize DataSz;
    uint8_t SignExtend;

    JumpType JmpType;

} Instruction;

typedef enum InstrEncDecErr
{

    INSTR_UNKNOWN,
    INSTR_WRONG_OPERANDS,
    INSTR_NOT_EXIST,
    INSTR_OK,

} InstrEncDecErr;

typedef struct ArgSet
{
    ArgType First;
    ArgType Second;

} ArgSet;

typedef const char *InstructionName;

struct InstructionMeta
{
    InstructionName Name;
    InstrOpCode OpCode;
    ArgSet ArgSets[8];
};

extern const InstructionMeta instructions[];

InstrEncDecErr NewInstruction(InstructionName name, Instruction *instr);

const InstructionMeta *FindInsMetaByOpCode(InstrOpCode opCode);

const InstructionMeta *FindInsMetaByName(InstructionName name);

#endif
