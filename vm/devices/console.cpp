#include <string.h>
#include "console.hpp"
#include "argument.hpp"

const size_t doubleMemAddr = 0;
const size_t intMemAddr = 8;
const size_t charMemAddr = 16;

int ConstructMajesticConsole(MajesticConsole *)
{

    return 0;
}

void DestructMajesticConsole(MajesticConsole *)
{

    return;
}

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz)
{

    // MajesticConsole *console = (MajesticConsole *)dev;

    if (addr == doubleMemAddr)
        scanf("%lf", data);
    else if (addr == intMemAddr)
        scanf("%ld", data);
    else if (addr == charMemAddr)
        scanf("%c", data);
    else
        return -1;

    return 0;
}

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize)
{

    // MajesticConsole *console = (MajesticConsole *)dev;

    if (addr == doubleMemAddr)
    {
        double d = 0;
        memcpy(&d, &data, sizeof(double));
        printf("%lf", d);
    }
    else if (addr == intMemAddr)
        printf("%ld", data);
    else if (addr == charMemAddr)
        printf("%c", data);
    else
        return -1;

    return 0;
}

void MajesticConsoleTicker(void *)
{

    return; // TODO
}
