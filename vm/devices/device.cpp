#include <assert.h>
#include "../vm.hpp"
#include "device.hpp"

Device *FindDevice(CPU *cpu, size_t addr)
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
