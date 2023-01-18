#include <stdio.h>
#include "vm.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong args count\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
        return 1;

    CPU cpu = {0};

    if (InitVM(&cpu, in) < 0)
    {
        DestructVM(&cpu);
        return -1;
    }

    RunVM(&cpu);
    DestructVM(&cpu);
    return 0;
}
