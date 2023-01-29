#include <stdio.h>
#include <stdlib.h>
#include "print_loadable.hpp"
#include "print_headers.hpp"

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

    ReadObj r = {0};
    r.in = in;
    r.out = stdout;

    if (printHeaders(&r) < 0)
        return EXIT_FAILURE;

    if (printLoadableSections(&r) < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
