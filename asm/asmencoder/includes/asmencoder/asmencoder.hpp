/** @file */
#ifndef ASM_ENCODER_HPP
#define ASM_ENCODER_HPP

#include <stdio.h>

#include "binformat.hpp"
#include "errors.hpp"
#include "parser/parser.hpp"

typedef struct AsmEncoder
{
    Parser* parser;
    FILE*   out;

    BinformatHeader header;

    SectionHeader* usrSectHdrs;
    size_t         usrSectionCount;

    SectionHeader* relocSectHdrs;
    size_t         relocSectionCount;

    uint32_t relocSectsBodyEnd;

    SectionHeader* symTabHdr;
    SectionHeader* strTabHdr;

    SymTabEntry* symTable;
    uint32_t     symTabSz;

    char*    strTab;
    uint32_t strTabSize;
    uint32_t strTabCapacity;

} AsmEncoder;

EncErrCode GenObjectFile(AsmEncoder* as);
void       AsmEncoderFree(AsmEncoder* as);

#endif
