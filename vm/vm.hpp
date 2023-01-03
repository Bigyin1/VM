/** @file */
#ifndef CPU_HPP
#define CPU_HPP

#include <stddef.h>
#include <stdio.h>

const size_t SecondaryDevicesCount = 2;

typedef FILE *(*ReaderFunc)(void *dev, size_t addr);
typedef FILE *(*WriterFunc)(void *dev, size_t addr);

typedef void (*DevTickFunc)(void *dev);

typedef struct Device
{
    const char *name;

    size_t lowAddr;
    size_t highAddr;

    void *concreteDevice;

    ReaderFunc getReader;
    WriterFunc getWriter;

    DevTickFunc tick;

} Device;

typedef struct CPU
{
    size_t regIP;
    uint64_t gpRegs[16];

    Device rom;
    Device ram;

    Device dev[SecondaryDevicesCount];
} CPU;

int InitVM(CPU *cpu, FILE *prog);

void DestructVM(CPU *cpu);

void RunVM(CPU *cpu);

#endif
