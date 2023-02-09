#include "readobj.hpp"

#include <stdlib.h>
#include <string.h>

const char* getNameFromStrTable(ReadObj* r, uint32_t nameIdx)
{
  if (nameIdx > r->strTableSize)
    return NULL;

  return r->strTable + nameIdx;
}

void readRelSection(ReadObj* r, SectionHeader* hdr)
{
  fseek(r->in, hdr->offset, SEEK_SET);

  r->currRelSectSz = hdr->size / (uint32_t)sizeof(RelEntry);
  r->currRelSect   = (RelEntry*)calloc(r->currRelSectSz, sizeof(RelEntry));

  fread(r->currRelSect, sizeof(RelEntry), r->currRelSectSz, r->in);
}

void getSectionRelocations(ReadObj* r, SectionHeader* hdr)
{
  if (r->fileHdr.fileType == BIN_EXEC)
    return;

  const char* sectName = getNameFromStrTable(r, hdr->nameIdx);

  for (uint32_t i = 0; i < r->fileHdr.sectionsCount; i++)
  {
    if (r->sectHdrs[i].type != SECT_REL)
      continue;

    const char* relSectName = getNameFromStrTable(r, r->sectHdrs[i].nameIdx);

    relSectName += strlen("rel.");  // TODO do something with that

    if (strcmp(relSectName, sectName) != 0)
      continue;

    long currOffset = ftell(r->in);
    fseek(r->in, r->sectHdrs[i].offset, SEEK_SET);

    readRelSection(r, &r->sectHdrs[i]);

    fseek(r->in, currOffset, SEEK_SET);
    return;
  }

  return;
}

void freeReadObj(ReadObj* r)
{
  free(r->sectHdrs);
  free(r->strTable);
  free(r->symTable);

  fclose(r->in);
  fclose(r->out);
}
