/** @file */
#ifndef LD_ARGS_HPP
#define LD_ARGS_HPP

#include <stdio.h>
#include "stdint.h"

typedef struct outSectionInfo
{
    const char *name;
    uint64_t addr;
} outSectionInfo;

typedef struct fileInfo
{
    FILE *file;
    const char *name;

} fileInfo;

typedef struct cmdArgs
{
    fileInfo files[8]; // TODO make dynamic
    uint16_t filesCount;

    outSectionInfo outSectInfo[8]; // TODO make dynamic
    uint16_t outSectsCount;

    FILE *out;
} cmdArgs;

int ParseCommandArgs(cmdArgs *args, int argc, char **argv);

#endif
