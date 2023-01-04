#include <string.h>
#include "registers.hpp"

const RegMeta regs[] = {

    {
        .Name = "r0",
        .RegCode = R0,
    },
    {
        .Name = "r1",
        .RegCode = R1,
    },
    {
        .Name = "r2",
        .RegCode = R2,
    },
    {
        .Name = "r3",
        .RegCode = R3,
    },
    {
        .Name = "r4",
        .RegCode = R4,
    },
    {
        .Name = "r5",
        .RegCode = R5,
    },
    {
        .Name = "rbp",
        .RegCode = RBP,
    },
    {
        .Name = "rsp",
        .RegCode = RSP,
    }};

int FindRegByName(RegName name)
{

    for (size_t i = 0; i < sizeof(regs) / sizeof(RegMeta); i++)
    {
        if (strcmp(regs[i].Name, name) == 0)
            return regs[i].RegCode;
    }

    return -1;
}
