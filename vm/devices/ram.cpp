#include <stdlib.h>
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
    ram->writer = fmemopen(ram->mem, ram->sz, "wb");

    setvbuf(ram->writer, NULL, _IONBF, 0);
    return 0;
}

void DestructRAM(RAM *ram)
{

    fclose(ram->reader);
    fclose(ram->writer);

    free(ram->mem);
}

FILE *RAMGetReaderOnAddr(void *ramP, size_t addr)
{
    RAM *ram = (RAM *)ramP;
    if (addr >= ram->sz)
        return NULL;

    fseek(ram->reader, addr, SEEK_SET);

    return ram->reader;
}

FILE *RAMGetWriterOnAddr(void *ramP, size_t addr)
{
    RAM *ram = (RAM *)ramP;
    if (addr >= ram->sz)
        return NULL;

    fseek(ram->writer, addr, SEEK_SET);

    return ram->writer;
}
