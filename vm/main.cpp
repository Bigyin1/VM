#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include "cpu/cpu.hpp"
#include "instructions/instructions.hpp"

static void readCode(const char *fName)
{
    assert(fName != NULL);

    FILE *f = fopen(fName, "r");
    if (f == NULL)
        return;

    Instruction instr = {0};
    while (!feof(f))
    {
        instr = {0};
        Decode(&instr, f);
    }

    fclose(f);
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Wrong args count\n");
        return 1;
    }

    readCode(argv[1]);
    // if (code == NULL)
    // {
    //     perror(argv[1]);
    //     return 1;
    // }

    // while (code->opCode != 0xC)
    // {
    //     printf("%s\n", findInstrByOpCode(code->opCode)->name);
    //     ++code;
    // }
}
