#include "vm.hpp"

#include <assert.h>
#include <stdlib.h>

#include "decode.hpp"
#include "instructions.hpp"
#include "registers.hpp"
#include "run.hpp"

static int execNextInstruction(CPU* cpu)
{
    Device* dev = FindDevice(cpu->devices, cpu->regIP);
    if (dev == NULL)
    {
        fprintf(stderr, "vm: unmapped address: %zu\n", cpu->regIP);
        return -1;
    }

    if (dev->getReader == NULL)
    {
        fprintf(stderr, "vm: device %s unable to execute code\n", dev->name);
        return -1;
    }

    FILE* reader = dev->getReader(dev->concreteDevice, cpu->regIP - dev->lowAddr);
    if (reader == NULL)
    {
        fprintf(stderr, "vm: device %s unable to serve execute request at address: %zu\n",
                dev->name, cpu->regIP);
        return -1;
    }

    long posPrev = ftell(reader);

    Instruction instr = {0};

    InstrCreationErr err = Decode(&instr, reader);
    if (err == INSTR_NOT_EXIST)
    {
        fprintf(stderr, "vm: bad instruction; finished executing\n");
        return -1;
    }
    if (err == INSTR_UNKNOWN)
    {
        fprintf(stderr, "vm: met unknown opcode\n");
        return -1;
    }
    if (err == INSTR_WRONG_OPERANDS)
    {
        fprintf(stderr, "vm: instruction %u: unknown arguments set\n", instr.im->OpCode);
        return -1;
    }

    cpu->regIP += (ftell(reader) - posPrev);

    if (Run(cpu, &instr) < 0)
        return -1;

    return 0;
}

void RunVM(CPU* cpu)
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
