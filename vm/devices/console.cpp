#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "console.hpp"
#include "argument.hpp"

const size_t doubleMemAddr = 0;
const size_t intMemAddr = 8;
const size_t charMemAddr = 16;
const size_t xCoordMemAddr = 17;
const size_t yCoordMemAddr = 19;
const size_t colorMemAddr = 21;

int ConstructMajesticConsole(MajesticConsole *con, FILE *r, FILE *w)
{
    struct stat st;
    if (fstat(3, &st) == 0)
        con->graphicsPipeFD = 3;

    con->r = r;
    con->w = w;
    setvbuf(con->w, NULL, _IONBF, 0);
    setvbuf(con->r, NULL, _IONBF, 0);
    return 0;
}

void DestructMajesticConsole(MajesticConsole *con)
{
    if (con == NULL)
        return;

    fclose(con->r);
    fclose(con->w);

    return;
}

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize sz)
{

    MajesticConsole *console = (MajesticConsole *)dev;

    if (addr == offsetof(MajesticConsoleMemMap, doubleInOut))
    {
        if (sz != DataWord)
            return -1;

        fscanf(console->r, "%lf", data);
        fscanf(console->r, "%*c"); // read out newline
    }
    else if (addr == offsetof(MajesticConsoleMemMap, intInOut))
    {
        fscanf(console->r, "%ld", data);
        fscanf(console->r, "%*c");
    }
    else if (addr == offsetof(MajesticConsoleMemMap, charInOut))
    {
        if (sz != DataByte)
            return -1;

        fscanf(console->r, "%c", data);
    }
    else
        return -1;

    return 0;
}

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize sz)
{

    MajesticConsole *console = (MajesticConsole *)dev;

    if (addr == offsetof(MajesticConsoleMemMap, doubleInOut))
    {
        if (sz != DataWord)
            return -1;

        double d = 0;
        memcpy(&d, &data, sizeof(double));
        fprintf(console->w, "%lf", d);
    }
    else if (addr == offsetof(MajesticConsoleMemMap, intInOut))
    {
        int64_t i = (int64_t)data;
        fprintf(console->w, "%ld", i);
    }
    else if (addr == offsetof(MajesticConsoleMemMap, charInOut))
    {
        if (sz != DataByte)
            return -1;

        char c = (char)data;
        fprintf(console->w, "%c", c);
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

        write(console->graphicsPipeFD, &console->mem.screenX,
              sizeof(console->mem.screenX) + sizeof(console->mem.screenY) + sizeof(console->mem.rgb));
        return 0;
    }

    else
        return -1;

    return 0;
}

void MajesticConsoleTicker(void *)
{

    return; // TODO
}
