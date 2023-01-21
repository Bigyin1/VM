#include <stdlib.h>
#include <string.h>
#include "ram.hpp"

int ConstructRAM(RAM *ram, size_t sz)
{

    ram->sz = sz;
    ram->mem = (char *)calloc(sz, 1);
    if (ram->mem == NULL)
    {
        perror("vm");
        return -1;
    }
    ram->reader = fmemopen(ram->mem, ram->sz, "r");

    ram->writer = fmemopen(ram->mem, ram->sz, "w");
    setvbuf(ram->writer, NULL, _IONBF, 0);

    return 0;
}

void DestructRAM(RAM *ram)
{

    fclose(ram->reader);
    fclose(ram->writer);

    free(ram->mem);
}

FILE *RAMGetReaderOnAddr(void *dev, size_t addr)
{
    RAM *ram = (RAM *)dev;
    if (addr >= ram->sz)
        return NULL;

    fseek(ram->reader, addr, SEEK_SET);

    return ram->reader;
}

FILE *RAMGetWriterOnAddr(void *dev, size_t addr)
{
    RAM *ram = (RAM *)dev;
    if (addr >= ram->sz)
        return NULL;

    fseek(ram->writer, addr, SEEK_SET);

    return ram->writer;
}

int RAMReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz)
{
    RAM *ram = (RAM *)dev;

    size_t toRead = DataSzToBytesSz(sz);
    if (addr + toRead > ram->sz)
        return -1;

    memcpy(data, ram->mem + addr, toRead);

    return 0;
}

int RAMWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz)
{

    RAM *ram = (RAM *)dev;

    size_t toRead = DataSzToBytesSz(sz);
    if (addr + toRead > ram->sz)
        return -1;

    memcpy(ram->mem, &data, toRead);

    return 0;
}
