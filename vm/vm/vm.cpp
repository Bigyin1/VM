#include <stdlib.h>
#include <assert.h>
#include "instructions.hpp"
#include "run.hpp"
#include "decode.hpp"
#include "registers.hpp"
#include "rom.hpp"
#include "ram.hpp"
#include "console.hpp"
#include "vm.hpp"

static bool checkMagic(FILE *prog)
{
    uint32_t magic = 0;
    fread(&magic, sizeof(magic), 1, prog);
    if (magic != 0xFAAFAAAF) /// TODO
    {
        printf("bad magic number in header\n");
        fclose(prog);
        return false;
    }
    return true;
}

static int attachROM(CPU *cpu, size_t addr, size_t sz)
{

    cpu->rom.lowAddr = addr;
    cpu->rom.highAddr = cpu->rom.lowAddr + sz - 1;

    cpu->rom.name = "ROM";

    cpu->rom.getReader = ROMGetReaderOnAddr;
    cpu->rom.getWriter = ROMGetWriterOnAddr;

    cpu->rom.concreteDevice = calloc(1, sizeof(ROM));

    if (ConstructROM((ROM *)cpu->rom.concreteDevice, sz) < 0)
        return -1;

    return 0;
}

static int attachRAM(CPU *cpu, size_t addr, size_t sz)
{

    cpu->ram.lowAddr = addr;
    cpu->ram.highAddr = cpu->ram.lowAddr + sz - 1;

    cpu->ram.name = "RAM";

    cpu->ram.getReader = RAMGetReaderOnAddr;
    cpu->ram.getWriter = RAMGetWriterOnAddr;

    cpu->ram.concreteDevice = calloc(1, sizeof(RAM));

    if (ConstructRAM((RAM *)cpu->ram.concreteDevice, sz) < 0)
        return -1;

    return 0;
}

static int attachConsole(CPU *cpu, size_t addr)
{

    cpu->dev[0].lowAddr = addr;
    cpu->dev[0].highAddr = cpu->dev[0].lowAddr + sizeof(double) + sizeof(int64_t) + sizeof(char) - 1;

    cpu->dev[0].name = "Majestic Console";

    cpu->dev[0].getReader = MajesticConsoleGetReaderOnAddr;
    cpu->dev[0].getWriter = MajesticConsoleGetWriterOnAddr;
    cpu->dev[0].tick = MajesticConsoleTicker;

    cpu->dev[0].concreteDevice = calloc(1, sizeof(MajesticConsole));

    if (ConstructMajesticConsole((MajesticConsole *)cpu->dev[0].concreteDevice) < 0)
        return -1;

    return 0;
}

int InitVM(CPU *cpu, FILE *prog)
{
    assert(cpu != NULL);

    if (!checkMagic(prog))
        return -1;

    if (attachROM(cpu, 0, 4096) < 0)
        return -1;

    if (attachRAM(cpu, 4096, 4096) < 0)
        return -1;

    if (attachConsole(cpu, 10000) < 0)
        return -1;

    cpu->regIP = cpu->rom.lowAddr;
    cpu->gpRegs[RSP] = cpu->ram.lowAddr; // SP

    if (LoadCode((ROM *)cpu->rom.concreteDevice, prog) < -1)
        return -1;

    return 0;
}

void DestructVM(CPU *cpu)
{
    assert(cpu != NULL);

    DestructRAM((RAM *)cpu->ram.concreteDevice);
    DestructROM((ROM *)cpu->rom.concreteDevice);
    DestructMajesticConsole((MajesticConsole *)cpu->dev[0].concreteDevice);

    free(cpu->ram.concreteDevice);
    free(cpu->rom.concreteDevice);
    free(cpu->dev[0].concreteDevice);
}

static int execNextInstruction(CPU *cpu)
{
    Device *dev = FindDevice(cpu->dev, cpu->regIP);
    if (dev == NULL)
    {
        printf("vm: unmapped address: %zu\n", cpu->regIP);
        return -1;
    }

    if (dev != &cpu->ram && dev != &cpu->rom)
    {
        printf("vm: trying to execute instruction from %s\n", dev->name);
        return -1;
    }

    FILE *reader = dev->getReader(dev->concreteDevice, cpu->regIP - dev->lowAddr);
    if (reader == NULL)
    {
        printf("vm: device %s unable to serve read request at address: %zu\n",
               dev->name, cpu->regIP - dev->lowAddr);
        return -1;
    }

    long posPrev = ftell(reader);

    Instruction instr = {0};

    InstrEncDecErr err = Decode(&instr, reader);
    if (err == INSTR_NOT_EXIST)
    {
        printf("vm: bad instruction; finished executing\n");
        return -1;
    }
    if (err == INSTR_UNKNOWN)
    {
        printf("vm: met unknown opcode\n");
        return -1;
    }
    if (err == INSTR_WRONG_OPERANDS)
    {
        printf("vm: instruction %u: unknown arguments set\n", instr.im->OpCode);
        return -1;
    }

    cpu->regIP += (ftell(reader) - posPrev);

    if (Run(cpu, &instr) < 0)
        return -1;

    return 0;
}

void RunVM(CPU *cpu)
{
    assert(cpu != NULL);
    assert(cpu->ram.concreteDevice != NULL);
    assert(cpu->rom.concreteDevice != NULL);

    cpu->running = true;

    while (cpu->running)
    {
        if (execNextInstruction(cpu) < 0)
            return;

        for (size_t i = 0; i < MaxDevices; i++)
        {
            cpu->dev[i].tick(cpu->dev[i].concreteDevice);
        }
    }
}
