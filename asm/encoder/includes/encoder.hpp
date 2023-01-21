/** @file */
#ifndef ASM_ENCODER_HPP
#define ASM_ENCODER_HPP

#include <stdio.h>
#include "parser/parser.hpp"
#include "../../errors.hpp"

const uint32_t magicHeader = 0xFAAFAAAF;

typedef struct AsmEncoder
{
    Parser *parser;
    FILE *out;

} AsmEncoder;

asm_ecode Encode(AsmEncoder *as);

#endif
