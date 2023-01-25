/** @file */
#ifndef ASM_ASSEMBLE_HPP
#define ASM_ASSEMBLE_HPP

#include <stdio.h>

typedef enum AsmErrCode
{
    ASM_OK,
    ASM_SYSTEM_ERROR,
    ASM_USER_ERROR,

} AsmErrCode;

AsmErrCode assemble(FILE *in, FILE *out);

#endif
