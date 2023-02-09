#include "console.hpp"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "argument.hpp"

int ConstructMajesticConsole(Device*                      conDev,
                             const MajesticConsoleConfig* config)
{
    conDev->lowAddr  = config->address;
    conDev->highAddr = conDev->lowAddr + MajesticConsoleMemSize - 1;

    MajesticConsole* con = (MajesticConsole*)calloc(1, sizeof(MajesticConsole));

    conDev->concreteDevice = con;

    conDev->name = "Majestic Console";

    conDev->readFrom = MajesticConsoleReadFrom;
    conDev->writeTo  = MajesticConsoleWriteTo;
    conDev->tick     = MajesticConsoleTicker;

    con->config = config;

    con->formattedIn = fdopen(config->consoleInFD, "r");
    setvbuf(con->formattedIn, NULL, _IONBF, 0);

    con->formattedOut = fdopen(config->consoleOutFD, "w");
    setvbuf(con->formattedOut, NULL, _IONBF, 0);

    return 0;
}

void DestructMajesticConsole(Device* conDev)
{
    MajesticConsole* con = (MajesticConsole*)conDev->concreteDevice;

    fclose(con->formattedIn);
    fclose(con->formattedOut);

    free(conDev->concreteDevice);

    return;
}

int MajesticConsoleReadFrom(void* dev, size_t addr, uint64_t* data, DataSize sz)
{

    MajesticConsole* console = (MajesticConsole*)dev;

    if (addr == offsetof(MajesticConsoleMemMap, doubleInOut))
    {
        if (sz != DataWord)
            return -1;

        fscanf(console->formattedIn, "%lf", data);
        fscanf(console->formattedIn, "%*c"); // read out newline
    }
    else if (addr == offsetof(MajesticConsoleMemMap, intInOut))
    {
        fscanf(console->formattedIn, "%ld", data);
        fscanf(console->formattedIn, "%*c");
    }
    else if (addr == offsetof(MajesticConsoleMemMap, charInOut))
    {
        if (sz != DataByte)
            return -1;

        fscanf(console->formattedIn, "%c", data);
    }
    else
        return -1;

    return 0;
}

int MajesticConsoleWriteTo(void* dev, size_t addr, uint64_t data, DataSize sz)
{

    MajesticConsole* console = (MajesticConsole*)dev;

    if (addr == offsetof(MajesticConsoleMemMap, doubleInOut))
    {
        if (sz != DataWord)
            return -1;

        double d = 0;
        memcpy(&d, &data, sizeof(double));
        fprintf(console->formattedOut, "%lf", d);
    }
    else if (addr == offsetof(MajesticConsoleMemMap, intInOut))
    {
        int64_t i = (int64_t)data;
        fprintf(console->formattedOut, "%ld", i);
    }
    else if (addr == offsetof(MajesticConsoleMemMap, charInOut))
    {
        if (sz != DataByte)
            return -1;

        char c = (char)data;
        fprintf(console->formattedOut, "%c", c);
    }
    else if (addr == offsetof(MajesticConsoleMemMap, screenX))
    {
        if (sz != DataDByte)
            return -1;

        console->mem.screenX = (uint16_t)data;
        return 0;
    }
    else if (addr == offsetof(MajesticConsoleMemMap, screenY))
    {
        if (sz != DataDByte)
            return -1;

        console->mem.screenY = (uint16_t)data;
        return 0;
    }

    else if (addr == offsetof(MajesticConsoleMemMap, rgb))
    {
        if (sz != DataHalfWord)
            return -1;

        memcpy(console->mem.rgb, &data, sizeof(console->mem.rgb));

        write(console->config->graphicsPixelOutFD, &console->mem.screenX,
              sizeof(console->mem.screenX) + sizeof(console->mem.screenY) +
                  sizeof(console->mem.rgb));
        return 0;
    }

    else
        return -1;

    return 0;
}

void MajesticConsoleTicker(void*)
{

    return; // TODO
}
