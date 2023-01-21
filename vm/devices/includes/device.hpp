/** @file */
#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include "argument.hpp"

typedef void (*DevTickFunc)(void *dev);

typedef struct Device
{
    const char *name;

    size_t lowAddr;
    size_t highAddr;

    void *concreteDevice;

    FILE *(*getReader)(void *dev, size_t addr);

    FILE *(*getWriter)(void *dev, size_t addr);

    int (*readFrom)(void *dev, size_t addr, uint64_t *data, DataSize sz);

    int (*writeTo)(void *dev, size_t addr, uint64_t data, DataSize sz);

    DevTickFunc tick; // TODO

} Device;

Device *FindDevice(Device *devices, size_t addr);

#endif
