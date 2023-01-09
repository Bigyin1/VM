#include "console.hpp"

const size_t doubleMemAddr = 0;
const size_t intMemAddr = 8;
const size_t charMemAddr = 16;

int ConstructMajesticConsole(MajesticConsole *con)
{

    con->reader = fmemopen(con->mem, sizeof(con->mem), "r");
    con->writer = fmemopen(con->mem, sizeof(con->mem), "wb");

    setvbuf(con->writer, NULL, _IONBF, 0);

    return 0;
}

void DestructMajesticConsole(MajesticConsole *con)
{

    fclose(con->reader);
    fclose(con->writer);
}

FILE *MajesticConsoleGetReaderOnAddr(void *con, size_t addr)
{
    MajesticConsole *console = (MajesticConsole *)con;

    if (addr == doubleMemAddr)
        scanf("%lf", console->mem + doubleMemAddr);
    else if (addr == intMemAddr)
        scanf("%ld", console->mem + intMemAddr);
    else if (addr == charMemAddr)
        scanf("%c", console->mem + charMemAddr);
    else
        return NULL;

    fseek(console->reader, addr, SEEK_SET);

    return console->reader;
}

FILE *MajesticConsoleGetWriterOnAddr(void *con, size_t addr)
{

    MajesticConsole *console = (MajesticConsole *)con;

    if (addr != doubleMemAddr && addr != intMemAddr && addr != charMemAddr)
        return NULL;

    fseek(console->writer, addr, SEEK_SET);

    console->_write = true;
    console->_writeIdx = addr;

    return console->writer;
}

void MajesticConsoleTicker(void *con)
{
    MajesticConsole *console = (MajesticConsole *)con;

    if (!console->_write)
        return;

    if (console->_writeIdx == doubleMemAddr)
        printf("%lf", *(double *)(console->mem + doubleMemAddr));
    else if (console->_writeIdx == intMemAddr)
        printf("%lld", *(int64_t *)(console->mem + intMemAddr));
    else if (console->_writeIdx == charMemAddr)
        printf("%c", *(console->mem + charMemAddr));
    else
        return;

    console->_write = false;
}
