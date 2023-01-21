/** @file */
#ifndef DEVICE_RAM_HPP
#define DEVICE_RAM_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "argument.hpp"

typedef struct RAM
{
    char *mem;
    size_t sz;

    FILE *reader;
    FILE *writer;

} RAM;

int ConstructRAM(RAM *ram, size_t sz);

void DestructRAM(RAM *ram);

int RAMReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int RAMWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

FILE *RAMGetReaderOnAddr(void *ram, size_t addr);

FILE *RAMGetWriterOnAddr(void *ram, size_t addr);

#endif
