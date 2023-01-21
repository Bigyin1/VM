/** @file */
#ifndef ASM_ENCODER_HPP
#define ASM_ENCODER_HPP

#include <stdio.h>
#include "parser/parser.hpp"

typedef struct AsmEncoder
{
    Parser *parser;
    FILE *out;

    size_t offset;

} AsmEncoder;

int GenObjectFile(AsmEncoder *as);

#endif
