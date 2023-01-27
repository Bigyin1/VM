#include <stdlib.h>
#include <assert.h>
#include "vmconfig/config.hpp"
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

static int attachROM(CPU *cpu, const ROMConfig *cfg)
{

    if (ConstructROM(&cpu->devices[romDevIdx], cfg) < 0)
        return -1;

    return 0;
}

static int attachRAM(CPU *cpu, const RAMConfig *cfg)
{

    if (ConstructRAM(&cpu->devices[ramDevIdx], cfg) < 0)
        return -1;

    return 0;
}

static int attachConsole(CPU *cpu, const MajesticConsoleConfig *cfg)
{

    if (ConstructMajesticConsole(&cpu->devices[consoleDevIdx], cfg) < 0)
        return -1;

    return 0;
}

int InitVM(CPU *cpu)
{
    assert(cpu != NULL);

    if (attachROM(cpu, &romConfig) < 0)
        return -1;

    if (attachRAM(cpu, &ramConfig) < 0)
        return -1;

    if (attachConsole(cpu, &consoleConfig) < 0)
        return -1;

    cpu->gpRegs[RSP] = cpu->devices[ramDevIdx].lowAddr; // Default SP

    return 0;
}

void DestructVM(CPU *cpu)
{
    assert(cpu != NULL);

    DestructRAM(&cpu->devices[ramDevIdx]);
    DestructROM(&cpu->devices[romDevIdx]);
    DestructMajesticConsole(&cpu->devices[consoleDevIdx]);
}
