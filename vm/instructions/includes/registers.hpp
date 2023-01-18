/** @file */
#ifndef CPU_REGISTERS_HPP
#define CPU_REGISTERS_HPP

#include <cstddef>
#include <stdint.h>

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

int FindRegByName(RegName name);

#endif