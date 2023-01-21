/** @file */
#ifndef BINARY_FORMAT_HPP
#define BINARY_FORMAT_HPP

#include <stdio.h>
#include "stdint.h"

const uint32_t magicHeader = 0xFAAFAAAF;
const uint16_t formatVersion = 1;

typedef enum SectionType
{

    SECT_LOAD,
    SECT_STR_TAB,

} SectionType;

#pragma pack(push, 1)

typedef struct SectionHeader
{

    uint64_t addr;
    uint32_t offset;
    uint32_t size;
    SectionType type;
    uint8_t nameIdx;

} SectionHeader;

typedef struct BinformatHeader
{
    uint64_t entrypoint;
    uint32_t magic;
    uint16_t version;
    uint8_t stringTableIdx;
    uint8_t sectionsCount;

} BinformatHeader;

#pragma pack(pop)

int getObjFileHeader(FILE *in, BinformatHeader *h);

SectionHeader *getSectionHeaders(FILE *in, size_t sectCount);

#endif
