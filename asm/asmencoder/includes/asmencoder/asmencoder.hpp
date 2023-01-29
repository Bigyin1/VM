/** @file */
#ifndef ASM_ENCODER_HPP
#define ASM_ENCODER_HPP

#include <stdio.h>
#include "parser/parser.hpp"
#include "binformat.hpp"
#include "errors.hpp"

typedef struct AsmEncoder
{
    Parser *parser;
    FILE *out;

    BinformatHeader header;

    SectionHeader *usrSectHdrs;
    size_t usrSectionCount;

    SectionHeader *relocSectHdrs;
    size_t relocSectionCount;

    uint32_t relocSectsBodyEnd;

    SectionHeader *symTabHdr;
    SectionHeader *strTabHdr;

    SymTabEntry *symTable;
    uint32_t symTabSz;

    char *strTab;
    uint32_t strTabCurrOffset;
    uint32_t strTabCapacity;

} AsmEncoder;

EncErrCode GenObjectFile(AsmEncoder *as);
void AsmEncoderFree(AsmEncoder *as);

#endif
