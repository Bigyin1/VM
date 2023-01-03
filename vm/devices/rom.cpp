#include <stdlib.h>
#include "rom.hpp"

int ConstructROM(ROM *rom, size_t sz)
{
    rom->sz = sz;
    rom->mem = (char *)calloc(sz, 1);
    if (rom->mem == NULL)
    {
        perror("vm");
        return -1;
    }
    rom->reader = fmemopen(rom->mem, rom->sz, "r");
    return 0;
}

void DestructROM(ROM *rom)
{

    fclose(rom->reader);

    free(rom->mem);
}

FILE *ROMGetWriterOnAddr(void *, size_t)
{
    return NULL;
}

FILE *ROMGetReaderOnAddr(void *romP, size_t addr)
{
    ROM *rom = (ROM *)romP;
    if (addr >= rom->sz)
        return NULL;

    fseek(rom->reader, addr, SEEK_SET);

    return rom->reader;
}

int LoadCode(ROM *rom, FILE *in)
{

    size_t codeSz = 0;
    fread(&codeSz, sizeof(codeSz), 1, in);

    if (codeSz > rom->sz)
        return -1;

    fread(rom->mem, codeSz, 1, in);
    return 0;
}
