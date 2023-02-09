#include <assert.h>
#include <stdlib.h>

#include "console.hpp"
#include "instructions.hpp"
#include "ram.hpp"
#include "registers.hpp"
#include "rom.hpp"
#include "vm.hpp"
#include "vmconfig/config.hpp"

const size_t romDevIdx     = 0;
const size_t ramDevIdx     = 1;
const size_t consoleDevIdx = 2;

#define DEVICE(idx) cpu->devices[idx]

static int attachROM(CPU* cpu, const ROMConfig* cfg)
{

    if (ConstructROM(&cpu->devices[romDevIdx], cfg) < 0)
        return -1;

    return 0;
}

static int attachRAM(CPU* cpu, const RAMConfig* cfg)
{

    if (ConstructRAM(&cpu->devices[ramDevIdx], cfg) < 0)
        return -1;

    return 0;
}

static int attachConsole(CPU* cpu, const MajesticConsoleConfig* cfg)
{

    if (ConstructMajesticConsole(&cpu->devices[consoleDevIdx], cfg) < 0)
        return -1;

    return 0;
}

int InitVM(CPU* cpu, const VMConfig* cfg)
{
    assert(cpu != NULL);

    if (cfg->attachROM && attachROM(cpu, &romConfig) < 0)
        return -1;

    if (cfg->attachRAM && attachRAM(cpu, &ramConfig) < 0)
        return -1;

    if (cfg->attachConsole && attachConsole(cpu, &consoleConfig) < 0)
        return -1;

    cpu->gpRegs[RSP] = cpu->devices[ramDevIdx].lowAddr; // Default SP

    return 0;
}

void DestructVM(CPU* cpu, const VMConfig* cfg)
{
    assert(cpu != NULL);
    if (cfg->attachRAM)
        DestructRAM(&cpu->devices[ramDevIdx]);
    if (cfg->attachROM)
        DestructROM(&cpu->devices[romDevIdx]);
    if (cfg->attachConsole)
        DestructMajesticConsole(&cpu->devices[consoleDevIdx]);
}
