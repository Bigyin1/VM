#include <stdio.h>
#include <stdlib.h>
#include "ld.hpp"
#include "link.hpp"

int main(int argc, char **argv)
{
    cmdArgs args = {0};

    if (ParseCommandArgs(&args, argc, argv) < 0)
        return EXIT_FAILURE;

    LD ld = {0};
    ld.args = &args;

    if (ReadFilesData(&ld) < 0)
    {
        FreeLD(&ld);
        return EXIT_FAILURE;
    }

    if (LinkFiles(&ld) < 0)
    {
        FreeLD(&ld);
        return EXIT_FAILURE;
    }

    FreeLD(&ld);

    return EXIT_SUCCESS;
}
