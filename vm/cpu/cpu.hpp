/** @file */
#ifndef CPU_HPP
#define CPU_HPP

#include <stddef.h>


const size_t gpRegsCount = 8;
const size_t maxDevicesCount = 3;


enum operation {
    OP_READ;
    OP_WRITE;
}

struct devise_s {
    char name[16];

    void *dev;

    size_t lowAddr;
    size_t highAddr;

    int (*)(void* dev, size_t addr, operation op) func;
};


typedef struct devise_s devise_s;






typedef int cpu_gp_reg_t;

struct cpu_s
{
    size_t regIP;
    size_t regSP;
    size_t regBP;
    cpu_gp_reg_t regs[gpRegsCount];


    devise_s dev[maxDevicesCount];

};

typedef struct cpu_s cpu_s;

#endif
