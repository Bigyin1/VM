/** @file */
#ifndef CPU_ARGUMENT_HPP
#define CPU_ARGUMENT_HPP

#include <cstddef>
#include <stdint.h>

typedef enum ArgType
{

    ArgNone,
    ArgRegister,               // r1
    ArgImm,                    // 123 or label
    ArgRegisterIndirect,       // [r1]
    ArgRegisterOffsetIndirect, // [r1+128]
    ArgImmIndirect,            // [128]
    ArgImmOffsetIndirect,      // [128+128]

} ArgType;

typedef struct Argument
{
    ArgType Type;

    uint64_t Imm;
    uint8_t RegNum;

    int16_t ImmDisp16;
    // uint8_t DispRegNum;

} Argument;

#endif
