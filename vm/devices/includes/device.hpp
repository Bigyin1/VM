/** @file */
#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>

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

Device *FindDevice(Device *devices, size_t addr);

#endif
