/** @file */
#ifndef CPU_ARGUMENT_HPP
#define CPU_ARGUMENT_HPP

#include <cstddef>
#include <stdint.h>


typedef enum ArgType {

    ArgRegister,
    ArgImm,
    ArgRegisterIndirect,
    ArgRegisterOffsetIndirect,
    ArgRegisterOffsetRegIndirect,
    ArgImmIndirect,
    ArgImmOffsetIndirect,
    ArgNone

} ArgType;

typedef struct Argument
{
    ArgType     Type;

    uint64_t    Imm;
    uint8_t     RegNum;

    int16_t     ImmDisp16;
    uint8_t     DispRegNum;

} Argument;




#endif


