/** @file */
#ifndef DEVICE_CONSOLE_HPP
#define DEVICE_CONSOLE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "argument.hpp"

typedef struct MajesticConsole
{
    char mem[sizeof(double) + sizeof(int64_t) + sizeof(char)];

} MajesticConsole;

int ConstructMajesticConsole(MajesticConsole *con);

void DestructMajesticConsole(MajesticConsole *con);

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

void MajesticConsoleTicker(void *con);

#endif
