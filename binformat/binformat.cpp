#include <stdio.h>
#include <stdlib.h>
#include "binformat.hpp"

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
