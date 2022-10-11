/** @file */
#ifndef CPU_INSTRUCTIONS_HPP
#define CPU_INSTRUCTIONS_HPP

#include <cstddef>
#include <stdbool.h>
#include <stdio.h>


const size_t instrArgsMaxCount = 2;


typedef enum e_arg_type {

    ARG_EMPTY = 1 << 0,
    ARG_IMM = 1 << 1,
    ARG_REG = 1 << 2,
    ARG_IMM_ADDR = 1 << 3,
    ARG_REG_ADDR = 1 << 4,

} e_arg_type;


typedef struct instrBin_s {

    unsigned char opCode;
    char arg1Type;
    unsigned short arg1Value;
    char arg2Type;
    unsigned short arg2Value;

} instrBin_s;



////////////// meta info


typedef struct regMeta {

    const char *name;
    char regCode;

} regMeta;


typedef unsigned short argType;


typedef struct instrMeta {

    const char *name;
    unsigned char opCode;
    argType allowedArgs[instrArgsMaxCount];

} instrMeta;



const regMeta *findRegByName(const char *name);

const instrMeta *findInstrByName(const char *name);


#endif
