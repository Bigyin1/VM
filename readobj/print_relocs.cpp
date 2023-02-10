#include "print_relocs.hpp"

#include <stdlib.h>

#include "readobj.hpp"

static int printRelSection(ReadObj* r, SectionHeader* hdr)
{

    readRelSection(r, hdr);

    const char* sectName = getNameFromStrTable(r, hdr->nameIdx);

    fprintf(r->out, "\nRelocation section \"%s\" at offset %u has %u elements:\n", sectName,
            hdr->offset, r->currRelSectSz);

    fprintf(r->out, "  Offset\tSymb. Value\tSymb. Name\n");

    for (uint32_t i = 0; i < r->currRelSectSz; i++)
    {
        SymTabEntry* symb = &r->symTable[r->currRelSect[i].symbolIdx];
        fprintf(r->out, "%09llu\t%011llu\t%s\n", r->currRelSect[i].offset, symb->value,
                getNameFromStrTable(r, symb->nameIdx));
    }

    free(r->currRelSect);
    r->currRelSectSz = 0;

    return 0;
}

int printRelocSections(ReadObj* r)
{

    for (uint16_t i = 0; i < r->fileHdr.sectionsCount; i++)
    {
        if (r->sectHdrs[i].type != SECT_REL)
            continue;

        if (printRelSection(r, &r->sectHdrs[i]) < 0)
            return -1;
    }

    return 0;
}
