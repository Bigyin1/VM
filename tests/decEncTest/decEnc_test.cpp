#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assemble/assemble.hpp"
#include "binformat.hpp"
#include "decode.hpp"
#include "testIns.hpp"

static bool argsEq(Argument* arg1, Argument* arg2)
{

    if (arg1->Type != arg2->Type)
    {
        printf("failed: different arg types: wanted: %u, got: %u", arg1->Type, arg2->Type);
        return false;
    }

    if (arg1->_immArgSz != arg2->_immArgSz)
    {
        printf("failed: different argument sizes: wanted: %u, got: %u",
               DataSzToBytesSz(arg1->_immArgSz), DataSzToBytesSz(arg2->_immArgSz));
        return false;
    }

    switch (arg1->Type)
    {
        case ArgRegister:
            if (arg1->RegNum != arg2->RegNum)
            {
                printf("failed: different register numbers: wanted: %u, got: %u", arg1->RegNum,
                       arg2->RegNum);
                return false;
            }
            break;

        case ArgRegisterIndirect:
            if (arg1->RegNum != arg2->RegNum)
            {
                printf("failed: different register numbers: wanted: %u, got: %u", arg1->RegNum,
                       arg2->RegNum);
                return false;
            }
            break;

        case ArgImm:
            if (memcmp(&arg1->Imm, &arg2->Imm, DataSzToBytesSz(arg1->_immArgSz)))
            {
                printf("failed: different immediates: wanted: %llu, got: %llu", arg1->Imm,
                       arg2->Imm);
                return false;
            }
            break;

        case ArgImmIndirect:
            if (arg1->Imm != arg2->Imm)
            {
                printf("failed: different immediates: wanted: %llu, got: %llu", arg1->Imm,
                       arg2->Imm);
                return false;
            }
            break;

        case ArgRegisterOffsetIndirect:
            if (arg1->RegNum != arg2->RegNum)
            {
                printf("failed: different register numbers: wanted: %u, got: %u", arg1->RegNum,
                       arg2->RegNum);
                return false;
            }
            if (arg1->ImmDisp16 != arg2->ImmDisp16)
            {
                printf("failed: different disp immediates: wanted: %d, got: %d", arg1->ImmDisp16,
                       arg2->ImmDisp16);
                return false;
            }
            break;

        default:
            break;
    }

    return true;
}

static bool instrEq(Instruction* ins1, Instruction* ins2)
{
    if (ins1->im != ins2->im)
    {
        printf("failed: wanted: %s, got: %s\n", ins1->im->Name, ins2->im->Name);
        return false;
    }
    if (ins1->ArgSetIdx != ins2->ArgSetIdx)
    {
        printf("failed: wrong arg set for %s; wanted: %u, got: %u\n", ins1->im->Name,
               ins1->ArgSetIdx, ins2->ArgSetIdx);
        return false;
    }
    if (ins1->DataSz != ins2->DataSz)
    {
        printf("failed: wrong data size for %s; wanted: %u, got: %u\n", ins1->im->Name,
               DataSzToBytesSz(ins1->DataSz), DataSzToBytesSz(ins2->DataSz));
        return false;
    }
    if (ins1->JmpType != ins2->JmpType)
    {
        printf("failed: wrong jump type for %s; wanted: %u, got: %u\n", ins1->im->Name,
               ins1->JmpType, ins2->JmpType);
        return false;
    }
    if (ins1->SignExt != ins2->SignExt)
    {
        printf("failed: wrong sign extention for %s; wanted: %u, got: %u\n", ins1->im->Name,
               ins1->SignExt, ins2->SignExt);
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

static FILE* getTextSectionData(char* buf, size_t sz)
{

    FILE* f = fmemopen(buf, sz, "r");

    BinformatHeader hdr = {0};

    if (getObjFileHeader(f, &hdr) < 0)
    {
        perror("getTextSectionData: ");
        fclose(f);
        return NULL;
    }

    if (hdr.magic != binMagicHeader)
    {
        printf("bad magic number in header\n");
        fclose(f);
        return NULL;
    }

    if (hdr.sectionsCount != 4)
    {
        printf("insufficient test code format\n");
        fclose(f);
        return NULL;
    }

    SectionHeader* sectHdrs = getSectionHeaders(f, hdr.sectionsCount);
    if (sectHdrs == NULL)
    {
        perror("getTextSectionData: ");
        fclose(f);
        return NULL;
    }

    SectionHeader* textSect   = sectHdrs + 0;
    size_t         textOffset = textSect->offset;
    size_t         textSz     = textSect->size;

    free(sectHdrs);
    fclose(f);

    f = fmemopen(buf + textOffset, textSz, "r");

    return f;
}

int main(int argc, char** argv)
{

    if (argc != 2)
        return EXIT_FAILURE;

    FILE* in = fopen(argv[1], "r");
    if (in == NULL)
    {
        perror("decEncTest");
        return 1;
    }

    char buf[2048] = {0};

    FILE* out = fmemopen(buf, sizeof(buf), "wb");
    if (out == NULL)
        return 1;

    if (assemble(in, out) != ASM_OK)
        return 1;

    FILE* f = NULL;
    if ((f = getTextSectionData(buf, sizeof(buf))) == NULL)
    {
        printf("FAILED\n");
        return 1;
    }

    Instruction      instr = {0};
    InstrCreationErr err   = INSTR_OK;

    size_t i = 0;
    while (!feof(f))
    {
        instr = {0};
        err   = Decode(&instr, f);
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
