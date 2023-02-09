/** @file */
#ifndef CPU_ARGUMENT_HPP
#define CPU_ARGUMENT_HPP

#include <stdint.h>

#include <cstddef>

typedef enum DataSize // TODO: add default invalid value
{
    DataWord = 0, // default value
    DataHalfWord,
    DataDByte,
    DataByte,

} DataSize;

typedef enum ArgType
{

    ArgNone,
    ArgRegister,               // r1
    ArgImm,                    // 123 or label
    ArgRegisterIndirect,       // [r1]
    ArgRegisterOffsetIndirect, // [r1+128]
    ArgImmIndirect,            // [128]

} ArgType;

typedef struct Argument
{
    ArgType Type;

    uint64_t Imm;
    uint8_t  RegNum;

    int16_t ImmDisp16;

    DataSize _immArgSz;

} Argument;

size_t DataSzToBytesSz(DataSize sz);

#endif
