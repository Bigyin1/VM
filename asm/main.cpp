#include <stdio.h>
#include <stdlib.h>
#include "assemble/assemble.hpp"

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("Wrong args count\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
    {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    FILE *out = fopen(argv[2], "w");
    if (out == NULL)
    {
        perror(argv[2]);
        return EXIT_FAILURE;
    }

    AsmErrCode err = assemble(in, out);
    if (err == ASM_SYSTEM_ERROR)
    {
        perror("asm");
        return EXIT_FAILURE;
    }
    if (err == ASM_USER_ERROR)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
