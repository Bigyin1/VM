/** @file */
#ifndef DEVICE_CONSOLE_HPP
#define DEVICE_CONSOLE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>

typedef struct MajesticConsole
{
    char mem[sizeof(double) + sizeof(int64_t) + sizeof(char)];
    bool _write;
    size_t _writeIdx;

    FILE *reader;
    FILE *writer;

} MajesticConsole;

int ConstructMajesticConsole(MajesticConsole *con);

void DestructMajesticConsole(MajesticConsole *con);

FILE *MajesticConsoleGetReaderOnAddr(void *con, size_t addr);

FILE *MajesticConsoleGetWriterOnAddr(void *con, size_t addr);

void MajesticConsoleTicker(void *con);

#endif
