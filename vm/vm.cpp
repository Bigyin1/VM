#include <stdlib.h>
#include <assert.h>
#include "../asm/assembler/assembler.hpp"
#include "devices/rom.hpp"
#include "devices/ram.hpp"
#include "vm.hpp"

static bool checkMagic(FILE *prog)
{
    uint32_t magic = 0;
    fread(&magic, sizeof(magic), 1, prog);
    if (magic != magicHeader)
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

int InitVM(CPU *cpu, FILE *prog)
{
    assert(cpu != NULL);

    if (!checkMagic(prog))
        return -1;

    if (attachROM(cpu, 0, 4096) < 0)
        return -1;

    if (attachRAM(cpu, 4096, 4096) < 0)
        return -1;

    cpu->regIP = cpu->rom.lowAddr;
    cpu->gpRegs[14] = cpu->ram.lowAddr; // SP

    if (LoadCode((ROM *)cpu->rom.concreteDevice, prog) < -1)
        return -1;

    return 0;
}

void DestructVM(CPU *cpu)
{
    assert(cpu != NULL);

    DestructRAM((RAM *)cpu->ram.concreteDevice);
    DestructROM((ROM *)cpu->rom.concreteDevice);

    free(cpu->ram.concreteDevice);
    free(cpu->rom.concreteDevice);
}

static Device *findDevice(CPU *cpu, size_t addr)
{
    assert(cpu != NULL);

    if (cpu->ram.lowAddr <= addr && cpu->ram.highAddr >= addr)
        return &cpu->ram;

    if (cpu->rom.lowAddr <= addr && cpu->rom.highAddr >= addr)
        return &cpu->rom;

    for (size_t i = 0; i < SecondaryDevicesCount; i++)
    {
        if (cpu->dev[i].concreteDevice == NULL)
            continue;

        if (cpu->dev[i].lowAddr <= addr && cpu->dev[i].highAddr >= addr)
            return &cpu->dev[i];
    }

    return NULL;
}

static int execNextInstruction(CPU *cpu)
{
    Device *dev = findDevice(cpu, cpu->regIP);
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

    InstrErr err = Decode(&instr, reader);
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

    return 0;
}

void RunVM(CPU *cpu)
{
    assert(cpu != NULL);
    assert(cpu->ram.concreteDevice != NULL);
    assert(cpu->rom.concreteDevice != NULL);

    while (1)
    {
        if (execNextInstruction(cpu) < 0)
            return;
    }
}
