
#include <stdlib.h>
#include "print_headers.hpp"
#include "binformat.hpp"

static int printFileHeader(BinformatHeader *hdr, FILE *out)
{
    if (hdr->magic != magicHeader)
    {
        fprintf(stderr, "bad magic header: %x\n", hdr->magic);
        return -1;
    }
    if (hdr->version != formatVersion)
    {
        fprintf(stderr, "insufficient binary file version: %d\n", hdr->version);
        return -1;
    }
    if (hdr->fileType != BIN_LINKABLE && hdr->fileType != BIN_EXEC)
    {
        fprintf(stderr, "unknown bin file type\n");
        return -1;
    }

    fprintf(out, "Binary header:\n");
    fprintf(out, "  Magic:\t%x\n", hdr->magic);
    fprintf(out, "  Version:\t%d\n", hdr->version);
    fprintf(out, "  Entrypoint:\t%lu\n", hdr->entrypoint);

    if (hdr->fileType == BIN_LINKABLE)
        fprintf(out, "  File type:\t%s\n", "Linkable");
    else
        fprintf(out, "  File type:\t%s\n", "Executable");

    return 0;
}

static const char *getSectTypeVerbose(SectionType sType)
{

    switch (sType)
    {
    case SECT_LOAD:
        return "LOADABLE";

    case SECT_STR_TAB:
        return "STRING TABLE";

    case SECT_SYM_TAB:
        return "SYMBOL TABLE";

    case SECT_REL:
        return "REL";

    default:
        break;
    }

    return NULL;
}

static int printSectionHeader(ReadObj *r, SectionHeader *sectHdr, uint16_t idx)
{

    const char *name = getNameFromStrTable(r, sectHdr->nameIdx);
    if (name == NULL)
    {
        fprintf(stderr, "section %d has no name in string table\n", idx);
        return -1;
    }

    const char *sectType = getSectTypeVerbose(sectHdr->type);
    if (sectType == NULL)
    {
        fprintf(stderr, "section %s has insufficient type\n", name);
        return -1;
    }

    fprintf(r->out, "[%2d]\t%s\t\t\t%s\t\t%lu\t\t%u\n", idx, name,
            sectType, sectHdr->addr, sectHdr->offset);
    return 0;
}

static int printSectionHeaders(ReadObj *r)
{

    fprintf(r->out, "\n\n  File has %d sections:\n", r->fileHdr.sectionsCount);

    fprintf(r->out, "[Idx]\tName\t\t\tType\t\tAddress\t\tOffset\n");

    for (uint16_t i = 0; i < r->fileHdr.sectionsCount; i++)
    {
        if (printSectionHeader(r, &r->sectHdrs[i], i) < 0)
            return -1;
    }
    return 0;
}

static int readStringTable(ReadObj *r)
{

    SectionHeader *strTabHdr = &r->sectHdrs[r->fileHdr.stringTableIdx];
    r->strTableSize = strTabHdr->size;

    long currOffset = ftell(r->in);
    fseek(r->in, strTabHdr->offset, SEEK_SET);

    r->strTable = (char *)calloc(strTabHdr->size, sizeof(char));
    if (r->strTable == NULL)
        return -1;

    if (fread(r->strTable, strTabHdr->size, sizeof(char), r->in) == 0)
        return -1;

    fseek(r->in, currOffset, SEEK_SET);

    return 0;
}

int printHeaders(ReadObj *r)
{
    if (getObjFileHeader(r->in, &r->fileHdr) == 1)
    {
        perror("readobj");
        return -1;
    }

    if (printFileHeader(&r->fileHdr, r->out) < 0)
        return -1;

    r->sectHdrs = getSectionHeaders(r->in, r->fileHdr.sectionsCount);
    if (r->sectHdrs == NULL)
    {
        perror("readobj");
        return -1;
    }

    if (readStringTable(r) < 0)
    {
        perror("readobj");
        return -1;
    }

    if (printSectionHeaders(r) < 0)
        return -1;

    return 0;
}
