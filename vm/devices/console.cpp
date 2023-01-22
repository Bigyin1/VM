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
        con->grPipeFD = 3;

    con->r = r;
    con->w = w;
    return 0;
}

void DestructMajesticConsole(MajesticConsole *con)
{
    fclose(con->r);
    fclose(con->w);

    return;
}

int MajesticConsoleReadFrom(void *dev, size_t addr, uint64_t *data, DataSize)
{

    MajesticConsole *console = (MajesticConsole *)dev;

    if (addr == doubleMemAddr)
        fscanf(console->r, "%lf", data);
    else if (addr == intMemAddr)
        fscanf(console->r, "%ld", data);
    else if (addr == charMemAddr)
        fscanf(console->r, "%c", data);
    else
        return -1;

    return 0;
}

int MajesticConsoleWriteTo(void *dev, size_t addr, uint64_t data, DataSize)
{

    MajesticConsole *console = (MajesticConsole *)dev;

    if (addr == doubleMemAddr)
    {
        double d = 0;
        memcpy(&d, &data, sizeof(double));
        fprintf(console->w, "%lf", d);
    }
    else if (addr == intMemAddr)
    {
        int64_t i = (int64_t)data;
        fprintf(console->w, "%ld", i);
    }
    else if (addr == charMemAddr)
    {
        char c = (char)data;
        fprintf(console->w, "%c", c);
    }
    else if (addr == xCoordMemAddr || addr == yCoordMemAddr)
        memcpy(console->mem + addr, &data, sizeof(uint16_t));

    else if (addr == colorMemAddr)
    {
        console->mem[addr] = char(data);
        if (write(console->grPipeFD, console->mem + xCoordMemAddr,
                  2 * sizeof(uint16_t) + sizeof(char)) == -1)
        {
            fprintf(stderr, "vm: console: broken graphics\n");
            return -1;
        }
    }

    else
        return -1;

    return 0;
}

void MajesticConsoleTicker(void *)
{

    return; // TODO
}
