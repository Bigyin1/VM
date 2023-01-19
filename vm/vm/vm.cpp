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

static size_t romDevIdx = 0;
static size_t ramDevIdx = 1;
static size_t consoleDevIdx = 2;

#define DEVICE(idx) cpu->devices[idx]

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

    DEVICE(romDevIdx).lowAddr = addr;
    DEVICE(romDevIdx).highAddr = DEVICE(romDevIdx).lowAddr + sz - 1;

    DEVICE(romDevIdx).name = "ROM";

    DEVICE(romDevIdx).getReader = ROMGetReaderOnAddr;
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
    DEVICE(ramDevIdx).readFrom = RAMReadFrom;
    DEVICE(ramDevIdx).writeTo = RAMWriteTo;

    DEVICE(ramDevIdx).concreteDevice = calloc(1, sizeof(RAM));

    if (ConstructRAM((RAM *)DEVICE(ramDevIdx).concreteDevice, sz) < 0)
        return -1;

    return 0;
}

static int attachConsole(CPU *cpu, size_t addr)
{

    DEVICE(consoleDevIdx).lowAddr = addr;
    DEVICE(consoleDevIdx).highAddr = DEVICE(consoleDevIdx).lowAddr + sizeof(double) + sizeof(int64_t) + sizeof(char) - 1;

    DEVICE(consoleDevIdx).name = "Majestic Console";

    DEVICE(consoleDevIdx).readFrom = MajesticConsoleReadFrom;
    DEVICE(consoleDevIdx).writeTo = MajesticConsoleWriteTo;
    DEVICE(consoleDevIdx).tick = MajesticConsoleTicker;

    DEVICE(consoleDevIdx).concreteDevice = calloc(1, sizeof(MajesticConsole));

    if (ConstructMajesticConsole((MajesticConsole *)DEVICE(consoleDevIdx).concreteDevice) < 0)
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

    cpu->regIP = DEVICE(romDevIdx).lowAddr;
    cpu->gpRegs[RSP] = DEVICE(ramDevIdx).lowAddr; // SP

    if (LoadCode((ROM *)DEVICE(romDevIdx).concreteDevice, prog) < -1)
        return -1;

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

static int execNextInstruction(CPU *cpu)
{
    Device *dev = FindDevice(cpu->devices, cpu->regIP);
    if (dev == NULL)
    {
        printf("vm: unmapped address: %zu\n", cpu->regIP);
        return -1;
    }

    if (dev->getReader == NULL)
    {
        printf("vm: device %s unable to execute code\n");
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

    cpu->running = true;

    while (cpu->running)
    {
        if (execNextInstruction(cpu) < 0)
            return;

        for (size_t i = 0; cpu->devices[i].name; i++)
        {
            if (cpu->devices[i].tick == NULL)
                continue;
            cpu->devices[i].tick(cpu->devices[i].concreteDevice);
        }
    }
}
