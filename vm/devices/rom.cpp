#include "rom.hpp"

#include <stdlib.h>
#include <string.h>

int ConstructROM(Device* romDev, const ROMConfig* config)
{
    romDev->lowAddr  = config->address;
    romDev->highAddr = romDev->lowAddr + config->size - 1;

    romDev->name = "ROM";

    romDev->getReader = ROMGetReaderOnAddr;
    romDev->getWriter = ROMGetWriterOnAddr;
    romDev->readFrom  = ROMReadFrom;
    romDev->writeTo   = ROMWriteTo;

    ROM* rom = (ROM*)calloc(1, sizeof(ROM));

    romDev->concreteDevice = rom;

    rom->config = config;
    rom->mem    = (char*)calloc(rom->config->size, 1);
    if (rom->mem == NULL)
    {
        perror("vm");
        return -1;
    }
    rom->reader = fmemopen(rom->mem, rom->config->size, "r");
    setvbuf(rom->reader, NULL, _IONBF, 0);

    rom->writer = fmemopen(rom->mem, rom->config->size, "w");
    setvbuf(rom->writer, NULL, _IONBF, 0);

    return 0;
}

void DestructROM(Device* romDev)
{

    ROM* rom = (ROM*)romDev->concreteDevice;

    fclose(rom->reader);
    fclose(rom->writer);

    free(rom->mem);

    free(romDev->concreteDevice);
}

FILE* ROMGetReaderOnAddr(void* dev, size_t addr)
{
    ROM* rom = (ROM*)dev;
    if (addr >= rom->config->size)
        return NULL;

    fseek(rom->reader, addr, SEEK_SET);

    return rom->reader;
}

FILE* ROMGetWriterOnAddr(void* dev, size_t addr)
{
    ROM* rom = (ROM*)dev;
    if (addr >= rom->config->size)
        return NULL;

    fseek(rom->writer, addr, SEEK_SET);

    return rom->writer;
}

int ROMReadFrom(void* dev, size_t addr, uint64_t* data, DataSize sz)
{
    ROM* rom = (ROM*)dev;

    size_t toRead = DataSzToBytesSz(sz);
    if (addr + toRead > rom->config->size)
        return -1;

    memcpy(data, rom->mem + addr, toRead);

    return 0;
}

int ROMWriteTo(void* dev, size_t addr, uint64_t data, DataSize sz)
{

    return -1;
}
