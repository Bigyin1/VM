/** @file */
#ifndef CPU_HPP
#define CPU_HPP

#include <stddef.h>
#include "devices/device.hpp"

const size_t SecondaryDevicesCount = 2;

typedef struct CPU
{
    size_t regIP;
    int8_t statusReg;
    uint64_t gpRegs[16];

    Device rom;
    Device ram;

    Device dev[SecondaryDevicesCount];
} CPU;

int InitVM(CPU *cpu, FILE *prog);

void DestructVM(CPU *cpu);

void RunVM(CPU *cpu);

Device *FindDevice(CPU *cpu, size_t addr);
#endif
