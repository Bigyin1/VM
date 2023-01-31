/** @file */
#ifndef CPU_INSTRUCTS_HPP
#define CPU_INSTRUCTS_HPP

#include <cstddef>
#include <stdint.h>

#include "argument.hpp"
#include "opcodes.hpp"

typedef enum JumpType // TODO: add default invalid value
{

    JumpUncond = 0, // default value
    JumpEQ,         // ==
    JumpNEQ,        // !=
    JumpG,          // >
    JumpGE,         // >=
    JumpL,          // <
    JumpLE,         // <=

} JumpType;

typedef enum SignExtend // TODO: add default invalid value
{

    NotSignExtended = 0, // default value
    SignExtended,

} SignExtend;

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

typedef struct Instruction
{
    const InstructionMeta *im;
    Argument Arg1;
    Argument Arg2;
    uint8_t ArgSetIdx;

    DataSize DataSz;
    SignExtend SignExt;

    JumpType JmpType;

} Instruction;

typedef enum InstrCreationErr
{

    INSTR_UNKNOWN,
    INSTR_WRONG_OPERANDS,
    INSTR_NOT_EXIST,
    INSTR_OK,

} InstrCreationErr;

InstrCreationErr NewInstruction(InstructionName name, Instruction *instr);

const InstructionMeta *FindInsMetaByOpCode(InstrOpCode opCode);

const InstructionMeta *FindInsMetaByName(InstructionName name);

#endif
