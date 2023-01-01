/** @file */
#ifndef CPU_INSTR_META_HPP
#define CPU_INSTR_META_HPP

#include <cstddef>
#include <stdint.h>
#include "argument.hpp"
#include "encode.hpp"
#include "decode.hpp"

////////////// meta info

typedef struct ArgSet
{
    ArgType First;
    ArgType Second;

} ArgSet;


typedef const char* InstructionName;

typedef struct InstructionMeta
{
    InstructionName Name;
    uint8_t         OpCode;
    ArgSet          ArgSets[8];
    EncFunc         EncFunc;
    DecFunc         DecFunc;


} InstructionMeta;


typedef const char* RegName;

typedef struct RegMeta {

    RegName Name;
    uint8_t RegCode;

} RegMeta;



const InstructionMeta *findInsMetaByName(InstructionName name);

const InstructionMeta *findInsMetaByOpCode(uint8_t opCode);

int findArgSetIdx(uint8_t opCode, ArgSet args);

int findRegByName(RegName name);






#endif


