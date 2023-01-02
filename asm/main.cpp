#include <stdio.h>
#include "assemble.hpp"

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("Wrong args count\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
    {
        perror(argv[1]);
        return NULL;
    }

    FILE *out = fopen(argv[2], "w");
    if (out == NULL)
    {
        perror(argv[2]);
        return 1;
    }

    if (assemble(in, out) == E_ASM_ERR)
        return 1;

    return 0;
}
