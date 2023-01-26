/** @file */
#ifndef DEVICE_CONSOLE_HPP
#define DEVICE_CONSOLE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "argument.hpp"

#pragma pack(push, 1)

typedef struct MajesticConsoleMemMap
{
    double doubleInOut;
    int64_t intInOut;
    char charInOut;

    uint16_t screenX;
    uint16_t screenY;
    char rgb[3];

} MajesticConsoleMemMap;
#pragma pack(pop)

const size_t MajesticConsoleMemSize = sizeof(MajesticConsoleMemMap);

typedef struct MajesticConsole
{
    MajesticConsoleMemMap mem;

    FILE *r;
    FILE *w;

    int graphicsPipeFD;

} MajesticConsole;

int ConstructMajesticConsole(MajesticConsole *con, FILE *r, FILE *w);

void DestructMajesticConsole(MajesticConsole *con);

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

void MajesticConsoleTicker(void *con);

#endif
