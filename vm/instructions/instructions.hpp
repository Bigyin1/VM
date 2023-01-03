/** @file */
#ifndef CPU_INSTR_META_HPP
#define CPU_INSTR_META_HPP

#include <cstddef>
#include <stdint.h>

#include <stdio.h>
#include "argument.hpp"
#include "../vm.hpp"

typedef struct InstructionMeta InstructionMeta;

typedef enum DataSize
{
    DataWord = 0,
    DataHalfWord,
    DataDByte,
    DataByte,

} DataSize;

typedef struct Instruction
{
    const InstructionMeta *im;
    Argument Arg1;
    Argument Arg2;
    uint8_t ArgSetIdx;

    uint8_t DataSz;
    uint8_t SignExtend;

} Instruction;

typedef enum InstrDecErr
{

    INSTR_UNKNOWN,
    INSTR_WRONG_OPERANDS,
    INSTR_NOT_EXIST,
    INSTR_OK,

} InstrDecErr;

typedef InstrDecErr (*DecFunc)(Instruction *, FILE *);
typedef size_t (*EncFunc)(Instruction *, FILE *, bool);
typedef int (*RunFunc)(CPU *, Instruction *);

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
    RunFunc runFunc;
};

typedef const char *RegName;

typedef enum RegCodes
{
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    RSP,
    RBP

} RegCodes;

typedef struct RegMeta
{
    RegName Name;
    uint8_t RegCode;

} RegMeta;

typedef enum InstrOpCode
{
    RET = 0,
    LD,
    ST,
    MOV,
    PUSH,
    POP,
    ADD,
    ADDF,
    SUB,
    SUBF,
    MUL,
    MULF,
    DIV,
    DIVF,
    JMP,
    JEQ,
    JNEQ,
    JG,
    JL,
    CALL,
    CMP,

} InstrOpCode;

extern const InstructionMeta instructions[];

int FindRegByName(RegName name);

InstrDecErr NewInstruction(InstructionName name, Instruction *instr, size_t *sz);

InstrDecErr Decode(Instruction *ins, FILE *r);

int Encode(Instruction *ins, FILE *w);

#endif
