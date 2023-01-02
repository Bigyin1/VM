#include <stdio.h>
#include <stdlib.h>
#include "../asm/assemble.hpp"
#include "../vm/instructions/instructions.hpp"
#include "../asm/assembler/assembler.hpp"

Instruction testIns[] = {

    {
        .im = &instructions[RET],
        .Arg1 = {.Type = ArgNone},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
    },

    {
        .im = &instructions[LD],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImmIndirect, .Imm = 123},
        .ArgSetIdx = 2,
    },

    {
        .im = &instructions[LD],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgImmIndirect, .Imm = 1},
        .ArgSetIdx = 2,
    },
    {
        .im = &instructions[ST],
        .Arg1 = {.Type = ArgRegister, .RegNum = 2},
        .Arg2 = {
            .Type = ArgRegisterOffsetIndirect,
            .RegNum = 3,
            .ImmDisp16 = 128,
        },
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ST],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegisterIndirect, .RegNum = 2},
        .ArgSetIdx = 0,
    },
    {
        .im = &instructions[MOV],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegister, .RegNum = 1},
        .ArgSetIdx = 0,
    },
    {
        .im = &instructions[MOV],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgImm, .Imm = 123},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[MOV],
        .Arg1 = {.Type = ArgRegister, .RegNum = 2},
        .Arg2 = {.Type = ArgImm, .Imm = 4659770375504729735U},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[MOV],
        .Arg1 = {.Type = ArgRegister, .RegNum = 4},
        .Arg2 = {.Type = ArgImm, .Imm = 29},
        .ArgSetIdx = 1,
    },

    {
        .im = &instructions[PUSH],
        .Arg1 = {.Type = ArgImm, .Imm = 13876515361109783347U},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[PUSH],
        .Arg1 = {.Type = ArgImm, .Imm = 61},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[PUSH],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
    },
    {
        .im = &instructions[PUSH],
        .Arg1 = {.Type = ArgImm, .Imm = 128},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[POP],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
    },

    {
        .im = &instructions[ADD],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegister, .RegNum = 1},
        .ArgSetIdx = 0,
    },

    {
        .im = &instructions[MUL],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegister, .RegNum = 2},
        .ArgSetIdx = 0,
    },
    {
        .im = &instructions[MULF],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgImm, .Imm = 4653142004841054208},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[DIVF],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImm, .Imm = 4611686018427387904},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[JMP],
        .Arg1 = {.Type = ArgImm, .Imm = 92},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
    },

};

static bool argsEq(Argument *arg1, Argument *arg2)
{

    if (arg1->Type != arg2->Type)
    {
        printf("failed: different arg types: wanted: %u, got: %u", arg1->Type, arg2->Type);
        return false;
    }

    switch (arg1->Type)
    {
    case ArgRegister:
        if (arg1->RegNum != arg2->RegNum)
        {
            printf("failed: different register numbers: wanted: %u, got: %u",
                   arg1->RegNum, arg2->RegNum);
            return false;
        }
        break;

    case ArgRegisterIndirect:
        if (arg1->RegNum != arg2->RegNum)
        {
            printf("failed: different register numbers: wanted: %u, got: %u",
                   arg1->RegNum, arg2->RegNum);
            return false;
        }
        break;

    case ArgImm:
        if (arg1->Imm != arg2->Imm)
        {
            printf("failed: different immediates: wanted: %llu, got: %llu",
                   arg1->Imm, arg2->Imm);
            return false;
        }
        break;

    case ArgImmIndirect:
        if (arg1->Imm != arg2->Imm)
        {
            printf("failed: different immediates: wanted: %llu, got: %llu",
                   arg1->Imm, arg2->Imm);
            return false;
        }
        break;

    case ArgRegisterOffsetIndirect:
        if (arg1->RegNum != arg2->RegNum)
        {
            printf("failed: different register numbers: wanted: %u, got: %u",
                   arg1->RegNum, arg2->RegNum);
            return false;
        }
        if (arg1->ImmDisp16 != arg2->ImmDisp16)
        {
            printf("failed: different disp immediates: wanted: %d, got: %d",
                   arg1->ImmDisp16, arg2->ImmDisp16);
            return false;
        }
        break;

    case ArgImmOffsetIndirect:
        if (arg1->Imm != arg2->Imm)
        {
            printf("failed: different immediates: wanted: %llu, got: %llu",
                   arg1->Imm, arg2->Imm);
            return false;
        }
        if (arg1->ImmDisp16 != arg2->ImmDisp16)
        {
            printf("failed: different disp immediates: wanted: %d, got: %d",
                   arg1->ImmDisp16, arg2->ImmDisp16);
            return false;
        }
        break;

    default:
        break;
    }

    return true;
}

static bool instrEq(Instruction *ins1, Instruction *ins2)
{
    if (ins1->im != ins2->im)
    {
        printf("failed: wanted: %s, got: %s\n", ins1->im->Name, ins2->im->Name);
        return false;
    }
    if (ins1->ArgSetIdx != ins2->ArgSetIdx)
    {
        printf("failed: wrong arg set for %s; wanted: %u, got: %u\n",
               ins1->im->Name, ins1->ArgSetIdx, ins2->ArgSetIdx);
        return false;
    }
    if (!argsEq(&ins1->Arg1, &ins2->Arg1))
    {
        printf("\t(argument 1)\n");
        return false;
    }
    if (!argsEq(&ins1->Arg2, &ins2->Arg2))
    {
        printf("\t(argument 2)\n");
        return false;
    }
    return true;
}

static FILE *checkMagic(char buf[1024])
{

    FILE *f = fmemopen(buf, 1024, "r");

    uint32_t magic = 0;
    fread(&magic, sizeof(magic), 1, f);
    if (magic != magicHeader)
    {
        printf("bad magic number in header\n");
        fclose(f);
        return NULL;
    }

    size_t codeSz = 0;
    fread(&codeSz, sizeof(codeSz), 1, f);
    fclose(f);

    f = fmemopen(buf, sizeof(magic) + sizeof(codeSz) + codeSz, "r");

    fseek(f, sizeof(magic) + sizeof(codeSz), SEEK_SET);
    return f;
}

int main()
{
    FILE *in = fopen("test.code", "r");
    if (in == NULL)
        return 1;

    char buf[1024] = {0};

    FILE *out = fmemopen(buf, sizeof(buf), "w");
    if (out == NULL)
        return 1;

    if (assemble(in, out) == E_ASM_ERR)
        return 1;

    FILE *f = NULL;
    if ((f = checkMagic(buf)) == NULL)
    {
        printf("FAILED\n");
        return 1;
    }

    Instruction instr = {0};
    InstrErr err = INSTR_OK;

    size_t i = 0;
    while (!feof(f))
    {
        instr = {0};
        err = Decode(&instr, f);
        if (err == INSTR_NOT_EXIST && feof(f))
            break;

        if (i == sizeof(testIns) / sizeof(Instruction))
        {
            printf("too many instructions in test file\nFAILED\n");
            fclose(f);

            return 1;
        }

        printf("%zu: %s:\t", i + 1, instr.im->Name);

        if (err != INSTR_OK)
        {
            printf("decoding error\nFAILED\n");
            fclose(f);

            return 1;
        }

        if (!instrEq(&testIns[i], &instr))
        {
            printf("FAILED\n");
            fclose(f);

            return 1;
        }
        printf("OK\n");

        i++;
    }

    fclose(f);

    printf("SUCCESS\n");
    return 0;
}
