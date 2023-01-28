/** @file */
#ifndef BINARY_FORMAT_HPP
#define BINARY_FORMAT_HPP

#include <stdio.h>
#include "stdint.h"

extern const uint32_t magicHeader;
extern const uint16_t formatVersion;

extern const uint16_t SHN_UNDEF;
extern const uint16_t SHN_ABS;

typedef enum SectionType
{

    SECT_LOAD,
    SECT_STR_TAB,
    SECT_SYM_TAB,
    SECT_REL,

} SectionType;

typedef enum BinFileType
{

    BIN_EXEC,
    BIN_LINKABLE,

} BinFileType;

#pragma pack(push, 1)

typedef struct SectionHeader
{

    uint64_t addr;   // addr to load; only for exec files
    uint32_t offset; // offset in file
    uint32_t size;
    SectionType type;
    uint32_t nameIdx; // section name idx in string table

} SectionHeader;

typedef struct SymTabEntry
{

    uint64_t value;         // symbol value; in linkable file represents offset from it's section start
    uint32_t nameIdx;       // idx in string table
    uint16_t sectHeaderIdx; /* idx of section, which defines symbol;
                            SHN_UNDEF if symbol is undefined; SHN_ABS for absolute(non-relocatable) symbols */

} SymTabEntry;

typedef struct RelEntry
{

    uint64_t offset;    // offset from relocation's section start
    uint32_t symbolIdx; // relevant symbol idx in symbol table

} RelEntry;

typedef struct BinformatHeader
{
    uint64_t entrypoint;
    uint32_t magic;
    uint16_t version;
    uint8_t fileType;

    uint16_t sectionsCount;
    uint16_t stringTableIdx;
    uint16_t symbolTableIdx;

} BinformatHeader;

#pragma pack(pop)

int getObjFileHeader(FILE *in, BinformatHeader *h);

SectionHeader *getSectionHeaders(FILE *in, size_t sectCount);

#endif
