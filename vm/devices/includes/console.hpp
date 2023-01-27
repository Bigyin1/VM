/** @file */
#ifndef DEVICE_CONSOLE_HPP
#define DEVICE_CONSOLE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "device.hpp"
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

typedef struct MajesticConsoleConfig
{
    size_t address;

    int consoleInFD;
    int consoleOutFD;
    int graphicsPixelOutFD;
    int graphicsScreenOutFD; // unused TODO
    int configuratonInFD;    // unused
    int userInfoOutFD;       // unused

} MajesticConsoleConfig;

typedef struct MajesticConsole
{
    MajesticConsoleMemMap mem;

    const MajesticConsoleConfig *config;

    FILE *formattedIn;
    FILE *formattedOut;

    int graphicsPipeFD;

} MajesticConsole;

int ConstructMajesticConsole(Device *conDev, const MajesticConsoleConfig *config);

void DestructMajesticConsole(Device *conDev);

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz);

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz);

void MajesticConsoleTicker(void *con);

#endif
