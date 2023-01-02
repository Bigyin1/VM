/** @file */
#ifndef ASM_ASSEMBLER_HPP
#define ASM_ASSEMBLER_HPP

#include <stdio.h>
#include "../parser/parser.hpp"
#include "../errors.hpp"

extern const uint32_t magicHeader;

typedef struct assembler_s
{
    codeNode *prog;
    FILE *out;

} assembler_s;

asm_ecode assemble(assembler_s *as);

#endif
