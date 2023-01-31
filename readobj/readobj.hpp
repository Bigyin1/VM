/** @file */
#ifndef READOBJ_READOBJ_HPP
#define READOBJ_READOBJ_HPP

#include <stdio.h>
#include "binformat.hpp"
#include "stdint.h"

typedef char *StringTable;

typedef struct ReadObj
{
    FILE *in;
    FILE *out;

    BinformatHeader fileHdr;
    SectionHeader *sectHdrs;
    StringTable strTable;
    uint32_t strTableSize;

    SymTabEntry *symTable;
    uint32_t symTabSz;

    RelEntry *currRelSect;
    uint32_t currRelSectSz;
} ReadObj;

const char *getNameFromStrTable(ReadObj *r, uint32_t nameIdx);

void readRelSection(ReadObj *r, SectionHeader *hdr);

void getSectionRelocations(ReadObj *r, SectionHeader *hdr);

void freeReadObj(ReadObj *r);

#endif
