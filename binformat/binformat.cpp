#include <stdio.h>
#include <stdlib.h>
#include "binformat.hpp"

const uint32_t binMagicHeader = 0xFAAFAAAF;
const uint16_t binFormatVersion = 1;

const uint16_t SHN_UNDEF = 0xFFFF;
const uint16_t SHN_ABS = 0xFFFE;

const uint8_t SYMB_GLOBAL = 1;
const uint8_t SYMB_LOCAL = 2;

int getObjFileHeader(FILE *in, BinformatHeader *h)
{

    if (fread(h, sizeof(*h), 1, in) == 0)
        return -1;

    return 0;
}

SectionHeader *getSectionHeaders(FILE *in, size_t sectCount)
{

    SectionHeader *sectHdrs = (SectionHeader *)calloc(sectCount, sizeof(SectionHeader));
    if (sectHdrs == NULL)
        return NULL;

    if (fread(sectHdrs, sizeof(*sectHdrs), sectCount, in) != sectCount)
    {
        free(sectHdrs);
        return NULL;
    }

    return sectHdrs;
}
