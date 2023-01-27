/** @file */
#ifndef DEVICE_ROM_HPP
#define DEVICE_ROM_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "device.hpp"
#include "argument.hpp"

typedef struct ROMConfig
{
    size_t address;
    size_t size;

} ROMConfig;

typedef struct ROM
{
    const ROMConfig *config;
    char *mem;

    FILE *reader;
    FILE *writer;

} ROM;

int ConstructROM(Device *rom, const ROMConfig *config);

void DestructROM(Device *rom);

FILE *ROMGetReaderOnAddr(void *rom, size_t addr);

FILE *ROMGetWriterOnAddr(void *rom, size_t addr);

int ROMReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int ROMWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

#endif
