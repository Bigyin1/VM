#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "instructions.hpp"


const regMeta regs[gpRegsCount] = {
    {
        .name = "r1",
        .regCode = 0x1,
    },
    {
        .name = "r2",
        .regCode = 0x2,
    },
    {
        .name = "r3",
        .regCode = 0x3,
    },
    {
        .name = "r4",
        .regCode = 0x4,
    }
};


const instrMeta instructions[] = {

    {
        .name = "push",
        .opCode = 0x1,
        .allowedArgs = {ARG_IMM | ARG_REG, ARG_EMPTY},
    },
    {
        .name = "pop",
        .opCode = 0x2,
        .allowedArgs = {ARG_REG, ARG_EMPTY},
    },
    {
        .name = "jmp",
        .opCode = 0x3,
        .allowedArgs = {ARG_IMM | ARG_REG, ARG_EMPTY},
    },
    {
        .name = "add",
        .opCode = 0x4,
        .allowedArgs = {ARG_EMPTY, ARG_EMPTY},
    },
    {
        .name = "sub",
        .opCode = 0x5,
        .allowedArgs = {ARG_EMPTY, ARG_EMPTY},
    },
    {
        .name = "div",
        .opCode = 0x6,
        .allowedArgs = {ARG_EMPTY, ARG_EMPTY},
    },
    {
        .name = "mul",
        .opCode = 0x7,
        .allowedArgs = {ARG_EMPTY, ARG_EMPTY},
    },
    {
        .name = "load",
        .opCode = 0x8,
        .allowedArgs = {ARG_REG, ARG_REG_ADDR | ARG_IMM_ADDR},
    },
    {
        .name = "str",
        .opCode = 0x9,
        .allowedArgs = {ARG_REG_ADDR | ARG_IMM_ADDR, ARG_REG},
    },
    {
        .name = "call",
        .opCode = 0xA,
        .allowedArgs = {ARG_REG | ARG_IMM, ARG_EMPTY},
    },
    {
        .name = "ret",
        .opCode = 0xB,
        .allowedArgs = {ARG_EMPTY, ARG_EMPTY},
    },
    {
        .name = "end",
        .opCode = 0xC,
        .allowedArgs = {ARG_EMPTY, ARG_EMPTY},
    },

};


const regMeta *findRegByName(const char *name) {
    assert(name != NULL);

    for (size_t i = 0; i < sizeof(regs)/sizeof(regMeta); i++) {

        if (strcmp(regs[i].name, name) == 0)
            return &regs[i];

    }

    return NULL;
}



const instrMeta *findInstrByName(const char *name) {
    assert(name != NULL);

    for (size_t i = 0; i < sizeof(instructions)/sizeof(instrMeta); i++) {

        if (strcmp(instructions[i].name, name) == 0)
            return &instructions[i];

    }

    return NULL;
}

const instrMeta *findInstrByOpCode(char opCode) {

    for (size_t i = 0; i < sizeof(instructions)/sizeof(instrMeta); i++) {

        if (opCode == instructions[i].opCode)
            return &instructions[i];

    }

    return NULL;
}

