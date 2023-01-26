#include <stdio.h>
#include <stdlib.h>
#include "vm.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong args count\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
        return EXIT_FAILURE;

    CPU cpu = {0};

    if (InitVM(&cpu, stdin, stdout) < 0)
    {
        DestructVM(&cpu);
        return EXIT_FAILURE;
    }

    if (LoadExeFile(&cpu, in) < 0)
    {
        DestructVM(&cpu);
        return EXIT_FAILURE;
    }

    RunVM(&cpu);

    DestructVM(&cpu);

    return EXIT_SUCCESS;
}
