/** @file */
#ifndef CPU_INSTR_META_HPP
#define CPU_INSTR_META_HPP

#include <cstddef>
#include <stdint.h>

#include <stdio.h>
#include "argument.hpp"

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

typedef enum InstrErr
{

    INSTR_UNKNOWN,
    INSTR_WRONG_OPERANDS,
    INSTR_NOT_EXIST,
    INSTR_OK,

} InstrErr;

typedef InstrErr (*DecFunc)(Instruction *, FILE *);
typedef size_t (*EncFunc)(Instruction *, FILE *, bool);

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

InstrErr NewInstruction(InstructionName name, Instruction *instr, size_t *sz);

InstrErr Decode(Instruction *ins, FILE *r);

int Encode(Instruction *ins, FILE *w);

#endif
