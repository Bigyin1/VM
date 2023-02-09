/** @file */
#ifndef LD_HPP
#define LD_HPP

#include <stdio.h>

#include "binformat.hpp"
#include "cmdargs.hpp"
#include "stdint.h"

typedef char* StringTable;

typedef struct RelSection
{
    uint32_t loadableSectHeaderIdx;
    uint32_t relSectHeaderIdx;

    RelEntry* relSectEnries;
    uint32_t  relSectSz;

} RelSection;

typedef struct LinkableFile
{
    BinformatHeader fileHdr;

    SectionHeader* sectHdrs;
    uint32_t       sectHdrsSz;

    StringTable strTable;
    uint32_t    strTableSz;

    SymTabEntry* symTable;
    uint32_t     symTabSz;

    RelSection* relSections;
    uint32_t    relSectionsSz;

} LinkableFile;

typedef struct ExecutableFile
{
    BinformatHeader fileHdr;

    SectionHeader* sectHdrs;

    StringTable strTable;
    uint32_t    strTableSz;
    uint32_t    strTabCap;

    SymTabEntry* symTable;
    uint32_t     symTabSz;
    uint32_t     symTabCurrIdx;

} ExecutableFile;

typedef struct LD
{
    cmdArgs* args;

    LinkableFile* files;

    ExecutableFile execFile;
} LD;

int ReadFilesData(LD* ld);

void FreeLD(LD* ld);
#endif
