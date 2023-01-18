/** @file */
#ifndef CPU_HPP
#define CPU_HPP

#include <stddef.h>
#include "device.hpp"

const size_t MaxDevices = 5;

typedef struct CPU
{
    size_t regIP;
    int8_t statusReg;
    uint64_t gpRegs[16];

    Device rom;
    Device ram;

    Device dev[MaxDevices]; // address space

    bool running;
} CPU;

#endif
