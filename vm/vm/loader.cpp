#include <stdlib.h>
#include "binformat.hpp"
#include "loader.hpp"

static int loadSection(CPU *cpu, SectionHeader *sectHdr, FILE *in)
{

    if (sectHdr->type != SECT_LOAD)
        return 0;

    Device *dev = FindDevice(cpu->devices, sectHdr->addr);
    if (dev == NULL)
    {
        printf("vm: failed to load section on unmapped address: %zu\n", sectHdr->addr);
        return -1;
    }

    if (dev->getWriter == NULL)
    {
        printf("vm: device %s unable to be used as storage\n", dev->name);
        return -1;
    }

    FILE *writer = dev->getWriter(dev->concreteDevice, sectHdr->addr - dev->lowAddr);
    if (writer == NULL)
    {
        printf("vm: failed to load section: device %s unable to serve write request at address: %zu\n",
               dev->name, sectHdr->addr - dev->lowAddr);
        return -1;
    }

    if (fseek(in, sectHdr->offset, SEEK_SET) < 0)
    {
        perror("asm: load section: ");
        return -1;
    }

    char *buf = (char *)calloc(sectHdr->size, 1);
    if (buf == NULL)
    {
        perror("asm: load section: ");
        return -1;
    }

    if (fread(buf, sectHdr->size, 1, in) != 1)
    {
        free(buf);
        perror("asm: load section: ");
        return -1;
    }

    if (fwrite(buf, sectHdr->size, 1, writer) != 1)
    {
        free(buf);
        perror("asm: load section: ");
        return -1;
    }

    free(buf);

    return 0;
}

int loadExeFile(CPU *cpu, FILE *in)
{
    BinformatHeader hdr = {0};
    if (getObjFileHeader(in, &hdr) < 0)
    {
        perror("asm: load exe file: ");
        return -1;
    }

    if (hdr.magic != magicHeader)
    {
        fprintf(stderr, "bad magic number in header\n");
        return -1;
    }

    if (hdr.version != formatVersion)
    {
        fprintf(stderr, "invalid version\n");
        return -1;
    }

    SectionHeader *sectHdrs = getSectionHeaders(in, hdr.sectionsCount);
    if (sectHdrs == NULL)
    {
        perror("asm: load exe file: ");
        return -1;
    }

    for (size_t i = 0; i < hdr.sectionsCount; i++)
    {

        if (loadSection(cpu, sectHdrs + i, in) < 0)
        {
            free(sectHdrs);
            return -1;
        }
    }

    free(sectHdrs);
    return 0;
}