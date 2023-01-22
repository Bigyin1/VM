/** @file */
#ifndef DEVICE_CONSOLE_HPP
#define DEVICE_CONSOLE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "argument.hpp"

const size_t MajesticConsoleMemSize = sizeof(double) +
                                      sizeof(int64_t) +
                                      sizeof(char) +
                                      sizeof(uint16_t) +
                                      sizeof(uint16_t) +
                                      sizeof(char);

typedef struct MajesticConsole
{
    char mem[MajesticConsoleMemSize];

    FILE *r;
    FILE *w;

    uint grPipeFD;

} MajesticConsole;

int ConstructMajesticConsole(MajesticConsole *con, FILE *r, FILE *w);

void DestructMajesticConsole(MajesticConsole *con);

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

void MajesticConsoleTicker(void *con);

#endif
