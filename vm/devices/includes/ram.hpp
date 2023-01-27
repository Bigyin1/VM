/** @file */
#ifndef DEVICE_RAM_HPP
#define DEVICE_RAM_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "device.hpp"
#include "argument.hpp"

typedef struct RAMConfig
{
    size_t address;
    size_t size;

} RAMConfig;

typedef struct RAM
{
    const RAMConfig *config;

    char *mem;

    FILE *reader;
    FILE *writer;

} RAM;

int ConstructRAM(Device *ramDev, const RAMConfig *config);

void DestructRAM(Device *ramDev);

int RAMReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int RAMWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

FILE *RAMGetReaderOnAddr(void *ram, size_t addr);

FILE *RAMGetWriterOnAddr(void *ram, size_t addr);

#endif
