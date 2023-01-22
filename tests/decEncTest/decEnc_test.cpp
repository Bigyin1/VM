#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binformat.hpp"
#include "assemble.hpp"
#include "instructions.hpp"
#include "decode.hpp"

Instruction testIns[] = {

    {
        .im = &instructions[ins_ret],
        .Arg1 = {.Type = ArgNone},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
    },

    {
        .im = &instructions[ins_ld],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImmIndirect, .Imm = 123},
        .ArgSetIdx = 2,
        .DataSz = DataHalfWord,
        .SignExtend = 1,
    },

    {
        .im = &instructions[ins_ld],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgImmIndirect, .Imm = 1},
        .ArgSetIdx = 2,
    },
    {
        .im = &instructions[ins_st],
        .Arg1 = {.Type = ArgRegister, .RegNum = 2},
        .Arg2 = {
            .Type = ArgRegisterOffsetIndirect,
            .RegNum = 3,
            .ImmDisp16 = 128,
        },
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ins_st],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegisterIndirect, .RegNum = 2},
        .ArgSetIdx = 0,
        .DataSz = DataDByte,
    },
    {
        .im = &instructions[ins_mov],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImm, .Imm = (uint64_t)-1, ._immArgSz = DataByte},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ins_mov],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImm, .Imm = (uint64_t)-1, ._immArgSz = DataByte},
        .ArgSetIdx = 1,
        .SignExtend = 1,
    },
    {
        .im = &instructions[ins_mov],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegister, .RegNum = 1},
        .ArgSetIdx = 0,
    },
    {
        .im = &instructions[ins_mov],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgImm, .Imm = 256, ._immArgSz = DataDByte},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ins_mov],
        .Arg1 = {.Type = ArgRegister, .RegNum = 2},
        .Arg2 = {.Type = ArgImm, .Imm = 4659770375504729735U, ._immArgSz = DataWord},
        .ArgSetIdx = 1,
    },

    {
        .im = &instructions[ins_push],
        .Arg1 = {.Type = ArgImm, .Imm = 13876515361109783347U, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 1,
        .DataSz = DataWord,
    },
    {
        .im = &instructions[ins_push],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .DataSz = DataWord,
    },
    {
        .im = &instructions[ins_push],
        .Arg1 = {.Type = ArgImm, .Imm = 128, ._immArgSz = DataByte},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 1,
        .DataSz = DataByte,
    },
    {
        .im = &instructions[ins_pop],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .DataSz = DataHalfWord,
    },

    {
        .im = &instructions[ins_add],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgRegister, .RegNum = 1},
        .ArgSetIdx = 0,
    },

    {
        .im = &instructions[ins_mul],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImm, .Imm = 3, ._immArgSz = DataByte},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ins_mulf],
        .Arg1 = {.Type = ArgRegister, .RegNum = 1},
        .Arg2 = {.Type = ArgImm, .Imm = 4653142004841054208, ._immArgSz = DataWord},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ins_divf],
        .Arg1 = {.Type = ArgRegister, .RegNum = 0},
        .Arg2 = {.Type = ArgImm, .Imm = 4611686018427387904, ._immArgSz = DataWord},
        .ArgSetIdx = 1,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpNEQ,
    },

    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 1, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },

    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 11, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 21, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpEQ,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 26, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 29, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 32, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 35, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 37, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },
    {
        .im = &instructions[ins_jmp],
        .Arg1 = {.Type = ArgImm, .Imm = 41, ._immArgSz = DataWord},
        .Arg2 = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType = JumpUncond,
    },

};

static bool argsEq(Argument *arg1, Argument *arg2)
{

    if (arg1->Type != arg2->Type)
    {
        printf("failed: different arg types: wanted: %u, got: %u", arg1->Type, arg2->Type);
        return false;
    }

    if (arg1->_immArgSz != arg2->_immArgSz)
    {
        printf("failed: different argument sizes: wanted: %u, got: %u", DataSzToBytesSz(arg1->_immArgSz),
               DataSzToBytesSz(arg2->_immArgSz));
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
        if (memcmp(&arg1->Imm, &arg2->Imm, DataSzToBytesSz(arg1->_immArgSz)))
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
    if (ins1->DataSz != ins2->DataSz)
    {
        printf("failed: wrong data size for %s; wanted: %u, got: %u\n",
               ins1->im->Name, DataSzToBytesSz(ins1->DataSz), DataSzToBytesSz(ins2->DataSz));
        return false;
    }
    if (ins1->JmpType != ins2->JmpType)
    {
        printf("failed: wrong jump type for %s; wanted: %u, got: %u\n",
               ins1->im->Name, ins1->JmpType, ins2->JmpType);
        return false;
    }
    if (ins1->SignExtend != ins2->SignExtend)
    {
        printf("failed: wrong sign extention for %s; wanted: %u, got: %u\n",
               ins1->im->Name, ins1->SignExtend, ins2->SignExtend);
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

static FILE *getTextSectionData(char *buf, size_t sz)
{

    FILE *f = fmemopen(buf, sz, "r");

    BinformatHeader hdr = {0};

    if (getObjFileHeader(f, &hdr) < 0)
    {
        perror("getTextSectionData: ");
        fclose(f);
        return NULL;
    }

    if (hdr.magic != magicHeader)
    {
        printf("bad magic number in header\n");
        fclose(f);
        return NULL;
    }

    if (hdr.sectionsCount != 2)
    {
        printf("insufficient test code format\n");
        fclose(f);
        return NULL;
    }

    SectionHeader *sectHdrs = getSectionHeaders(f, hdr.sectionsCount);
    if (sectHdrs == NULL)
    {
        perror("getTextSectionData: ");
        fclose(f);
        return NULL;
    }

    SectionHeader *textSect = sectHdrs + 0;

    free(sectHdrs);
    fclose(f);

    f = fmemopen(buf + textSect->offset, textSect->size, "r");

    return f;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong count of arguments\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
    {
        perror("decEncTest: ");
        return 1;
    }

    char buf[2048] = {0};

    FILE *out = fmemopen(buf, sizeof(buf), "wb");
    if (out == NULL)
        return 1;

    if (assemble(in, out) < 0)
        return 1;

    FILE *f = NULL;
    if ((f = getTextSectionData(buf, sizeof(buf))) == NULL)
    {
        printf("FAILED\n");
        return 1;
    }

    Instruction instr = {0};
    InstrEncDecErr err = INSTR_OK;

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
