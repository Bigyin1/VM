#include "cmdargs.hpp"

#include <string.h>

static bool tryFile(cmdArgs* args, const char* arg)
{
    size_t argLen = strlen(arg);

    int wordLen = 0;

    sscanf(arg, "%*[^-]%n", &wordLen);

    if ((size_t)wordLen != argLen)
        return false;

    FILE* f = fopen(arg, "r");
    if (f == NULL)
    {
        fprintf(stderr, "can't open file %zu\n", argLen);
        return false;
    }

    args->files[args->filesCount].file = f;
    args->files[args->filesCount].name = arg;
    args->filesCount++;
    return true;
}

static bool trySectionInfo(cmdArgs* args, char* arg)
{
    size_t argLen = strlen(arg);

    unsigned wordLen     = 0;
    unsigned sectNameLen = 0;

    uint64_t sectAddr = 0;

    if (sscanf(arg, "--%*[^=]%n=%llu%n", (int*)&sectNameLen, &sectAddr, (int*)&wordLen) == 0)
        return false;

    if ((size_t)wordLen != argLen)
        return false;

    if (sectNameLen == 0)
        return false;

    arg[sectNameLen] = '\0';

    args->outSectInfo[args->outSectsCount].name = arg + 2;
    args->outSectInfo[args->outSectsCount].addr = sectAddr;

    args->outSectsCount++;

    return true;
}

static int parserArg(cmdArgs* args, char* arg)
{
    if (tryFile(args, arg))
        return 0;

    if (trySectionInfo(args, arg))
        return 0;

    fprintf(stderr, "failed to parser args");

    return -1;
}

static int checkSectInfoDublicates(cmdArgs* args)
{
    for (size_t i = 0; i < args->outSectsCount; i++)
    {
        const char* curr = args->outSectInfo[i].name;

        for (size_t j = i + 1; j < args->outSectsCount; j++)
        {
            if (strcmp(curr, args->outSectInfo[j].name) == 0)
            {
                fprintf(stderr, "found dublicates in section info args\n");
                return -1;
            }
        }
    }

    return 0;
}

int ParseCommandArgs(cmdArgs* args, int argc, char** argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "wrong args count, add a little bit more\n");
        return -1;
    }

    for (size_t i = 1; i < (size_t)argc - 1; i++)
    {
        char* arg = argv[i];
        if (parserArg(args, arg) < 0)
            return -1;
    }

    if (checkSectInfoDublicates(args) < 0)
        return -1;

    if (args->filesCount == 0)
    {
        fprintf(stderr, "no files provided");
        return -1;
    }

    if (args->outSectsCount == 0)
    {
        fprintf(stderr, "no sections info provided");
        return -1;
    }

    char* outFileName = argv[argc - 1];

    args->out = fopen(outFileName, "w");
    if (args->out == NULL)
    {
        perror("ld");
        return -1;
    }

    return 0;
}
