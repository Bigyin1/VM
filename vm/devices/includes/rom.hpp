/** @file */
#ifndef DEVICE_ROM_HPP
#define DEVICE_ROM_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>

typedef struct ROM
{
    char *mem;
    size_t sz;

    FILE *reader;

} ROM;

int ConstructROM(ROM *rom, size_t sz);

void DestructROM(ROM *rom);

FILE *ROMGetReaderOnAddr(void *rom, size_t addr);

FILE *ROMGetWriterOnAddr(void *rom, size_t addr);

int LoadCode(ROM *rom, FILE *in);

#endif
