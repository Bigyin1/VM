#include <stdlib.h>
#include <string.h>
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
    rom->writer = fmemopen(rom->mem, rom->sz, "w");

    setvbuf(rom->writer, NULL, _IONBF, 0);

    return 0;
}

void DestructROM(ROM *rom)
{

    fclose(rom->reader);
    fclose(rom->writer);

    free(rom->mem);
}

FILE *ROMGetReaderOnAddr(void *dev, size_t addr)
{
    ROM *rom = (ROM *)dev;
    if (addr >= rom->sz)
        return NULL;

    fseek(rom->reader, addr, SEEK_SET);

    return rom->reader;
}

FILE *ROMGetWriterOnAddr(void *dev, size_t addr)
{
    ROM *rom = (ROM *)dev;
    if (addr >= rom->sz)
        return NULL;

    fseek(rom->writer, addr, SEEK_SET);

    return rom->writer;
}

int ROMReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz)
{
    ROM *ram = (ROM *)dev;

    size_t toRead = DataSzToBytesSz(sz);
    if (addr + toRead > ram->sz)
        return -1;

    memcpy(data, ram->mem + addr, toRead);

    return 0;
}

int ROMWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz)
{

    return -1;
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
