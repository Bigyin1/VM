/** @file */
#ifndef DEVICE_ROM_HPP
#define DEVICE_ROM_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "argument.hpp"

typedef struct ROM
{
    char *mem;
    size_t sz;

    FILE *reader;
    FILE *writer;

} ROM;

int ConstructROM(ROM *rom, size_t sz);

void DestructROM(ROM *rom);

FILE *ROMGetReaderOnAddr(void *rom, size_t addr);

FILE *ROMGetWriterOnAddr(void *rom, size_t addr);

int ROMReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int ROMWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

#endif
