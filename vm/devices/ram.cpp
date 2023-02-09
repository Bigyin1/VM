#include "ram.hpp"

#include <stdlib.h>
#include <string.h>

int ConstructRAM(Device* ramDev, const RAMConfig* config)
{
    ramDev->lowAddr  = config->address;
    ramDev->highAddr = ramDev->lowAddr + config->size - 1;

    ramDev->name = "RAM";

    RAM* ram = (RAM*)calloc(1, sizeof(RAM));

    ramDev->concreteDevice = ram;

    ramDev->getReader = RAMGetReaderOnAddr;
    ramDev->getWriter = RAMGetWriterOnAddr;
    ramDev->readFrom  = RAMReadFrom;
    ramDev->writeTo   = RAMWriteTo;

    ram->config = config;
    ram->mem    = (char*)calloc(ram->config->size, 1);
    if (ram->mem == NULL)
    {
        perror("vm");
        return -1;
    }
    ram->reader = fmemopen(ram->mem, ram->config->size, "r");
    setvbuf(ram->reader, NULL, _IONBF, 0);

    ram->writer = fmemopen(ram->mem, ram->config->size, "w");
    setvbuf(ram->writer, NULL, _IONBF, 0);

    return 0;
}

void DestructRAM(Device* ramDev)
{

    RAM* ram = (RAM*)ramDev->concreteDevice;

    fclose(ram->reader);
    fclose(ram->writer);
    free(ram->mem);

    free(ramDev->concreteDevice);
}

FILE* RAMGetReaderOnAddr(void* dev, size_t addr)
{
    RAM* ram = (RAM*)dev;
    if (addr >= ram->config->size)
        return NULL;

    fseek(ram->reader, addr, SEEK_SET);

    return ram->reader;
}

FILE* RAMGetWriterOnAddr(void* dev, size_t addr)
{
    RAM* ram = (RAM*)dev;
    if (addr >= ram->config->size)
        return NULL;

    fseek(ram->writer, addr, SEEK_SET);

    return ram->writer;
}

int RAMReadFrom(void* dev, size_t addr, uint64_t* data, DataSize sz)
{
    RAM* ram = (RAM*)dev;

    size_t toRead = DataSzToBytesSz(sz);
    if (addr + toRead > ram->config->size)
        return -1;

    memcpy(data, ram->mem + addr, toRead);

    return 0;
}

int RAMWriteTo(void* dev, size_t addr, uint64_t data, DataSize sz)
{

    RAM* ram = (RAM*)dev;

    size_t toWrite = DataSzToBytesSz(sz);
    if (addr + toWrite > ram->config->size)
        return -1;

    memcpy(ram->mem + addr, &data, toWrite);

    return 0;
}
