#include <stdlib.h>
#include <assert.h>
#include "instructions.hpp"
#include "registers.hpp"
#include "rom.hpp"
#include "ram.hpp"
#include "console.hpp"
#include "vm.hpp"

const size_t romDevIdx = 0;
const size_t ramDevIdx = 1;
const size_t consoleDevIdx = 2;

#define DEVICE(idx) cpu->devices[idx]

static int attachROM(CPU *cpu, size_t addr, size_t sz)
{

    DEVICE(romDevIdx).lowAddr = addr;
    DEVICE(romDevIdx).highAddr = DEVICE(romDevIdx).lowAddr + sz - 1;

    DEVICE(romDevIdx).name = "ROM";

    DEVICE(romDevIdx).getReader = ROMGetReaderOnAddr;
    DEVICE(romDevIdx).getWriter = ROMGetWriterOnAddr;
    DEVICE(romDevIdx).readFrom = ROMReadFrom;
    DEVICE(romDevIdx).writeTo = ROMWriteTo;

    DEVICE(romDevIdx).concreteDevice = calloc(1, sizeof(ROM));

    if (ConstructROM((ROM *)DEVICE(romDevIdx).concreteDevice, sz) < 0)
        return -1;

    return 0;
}

static int attachRAM(CPU *cpu, size_t addr, size_t sz)
{

    DEVICE(ramDevIdx).lowAddr = addr;
    DEVICE(ramDevIdx).highAddr = DEVICE(ramDevIdx).lowAddr + sz - 1;

    DEVICE(ramDevIdx).name = "RAM";

    DEVICE(ramDevIdx).getReader = RAMGetReaderOnAddr;
    DEVICE(ramDevIdx).getWriter = RAMGetWriterOnAddr;
    DEVICE(ramDevIdx).readFrom = RAMReadFrom;
    DEVICE(ramDevIdx).writeTo = RAMWriteTo;

    DEVICE(ramDevIdx).concreteDevice = calloc(1, sizeof(RAM));

    if (ConstructRAM((RAM *)DEVICE(ramDevIdx).concreteDevice, sz) < 0)
        return -1;

    return 0;
}

static int attachConsole(CPU *cpu, size_t addr, FILE *r, FILE *w)
{

    DEVICE(consoleDevIdx).lowAddr = addr;
    DEVICE(consoleDevIdx).highAddr = DEVICE(consoleDevIdx).lowAddr +
                                     MajesticConsoleMemSize - 1;

    DEVICE(consoleDevIdx).name = "Majestic Console";

    DEVICE(consoleDevIdx).readFrom = MajesticConsoleReadFrom;
    DEVICE(consoleDevIdx).writeTo = MajesticConsoleWriteTo;
    DEVICE(consoleDevIdx).tick = MajesticConsoleTicker;

    DEVICE(consoleDevIdx).concreteDevice = calloc(1, sizeof(MajesticConsole));

    if (ConstructMajesticConsole((MajesticConsole *)DEVICE(consoleDevIdx).concreteDevice, r, w) < 0)
        return -1;

    return 0;
}

int InitVM(CPU *cpu, FILE *consIn, FILE *consOut)
{
    assert(cpu != NULL);

    if (attachROM(cpu, 0, 4096) < 0) // TODO: add user's configuration for devices
        return -1;

    if (attachRAM(cpu, 4096, 4096) < 0)
        return -1;

    if (consIn != NULL && consOut != NULL)
        if (attachConsole(cpu, 10000, consIn, consOut) < 0)
            return -1;

    cpu->regIP = DEVICE(romDevIdx).lowAddr;
    cpu->gpRegs[RSP] = DEVICE(ramDevIdx).lowAddr; // SP

    return 0;
}

void DestructVM(CPU *cpu)
{
    assert(cpu != NULL);

    DestructRAM((RAM *)DEVICE(ramDevIdx).concreteDevice);
    DestructROM((ROM *)DEVICE(romDevIdx).concreteDevice);
    DestructMajesticConsole((MajesticConsole *)DEVICE(consoleDevIdx).concreteDevice);

    free(DEVICE(ramDevIdx).concreteDevice);
    free(DEVICE(romDevIdx).concreteDevice);
    free(DEVICE(consoleDevIdx).concreteDevice);
}