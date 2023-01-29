#include <stdlib.h>
#include <string.h>
#include "readobj.hpp"

const char *getNameFromStrTable(ReadObj *r, uint32_t nameIdx)
{
    if (nameIdx > -r->strTableSize)
        return NULL;

    return r->strTable + nameIdx;
}

RelEntry *getSectionRelocations(ReadObj *r, SectionHeader *hdr, uint32_t *sz)
{

    if (r->fileHdr.fileType == BIN_EXEC)
        return NULL;

    const char *sectName = getNameFromStrTable(r, hdr->nameIdx);

    for (uint32_t i = 0; i < r->fileHdr.sectionsCount; i++)
    {
        if (r->sectHdrs[i].type != SECT_REL)
            continue;

        const char *relSectName = getNameFromStrTable(r, r->sectHdrs[i].nameIdx);

        relSectName += strlen("rel."); // TODO do something with that

        if (strcmp(relSectName, sectName) != 0)
            continue;

        *sz = r->sectHdrs[i].size / sizeof(RelEntry);
        RelEntry *relSect = (RelEntry *)calloc(*sz, sizeof(RelEntry));

        long currOffset = ftell(r->in);
        fseek(r->in, r->sectHdrs[i].offset, SEEK_SET);

        fread(relSect, *sz, sizeof(RelEntry), r->in);

        fseek(r->in, currOffset, SEEK_SET);
        return relSect;
    }

    return NULL;
}

void freeReadObj(ReadObj *r)
{
    free(r->sectHdrs);
    free(r->strTable);
    free(r->symTable);

    fclose(r->in);
    fclose(r->out);
}
