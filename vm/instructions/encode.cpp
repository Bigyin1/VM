#include <ctype.h>
#include <cstddef>
#include "encode.hpp"

void encodeCommon(Argument *arg, FILE *w)
{

    switch (arg->Type)
    {
    case ArgRegister:
        fwrite(&arg->RegNum, 1, 1, w);
        break;

    case ArgRegisterIndirect:
        fwrite(&arg->RegNum, 1, 1, w);
        break;

    case ArgImmIndirect:
        fwrite(&arg->Imm, sizeof(arg->Imm), 1, w);
        break;

    case ArgRegisterOffsetIndirect:
        fwrite(&arg->RegNum, 1, 1, w);
        fwrite(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, w);
        break;

    case ArgRegisterOffsetRegIndirect:
    {
        uint8_t byte = arg->RegNum | (arg->DispRegNum << 4);
        fwrite(&byte, 1, 1, w);
        break;
    }

    case ArgImmOffsetIndirect:
        fwrite(&arg->Imm, sizeof(arg->Imm), 1, w);
        fwrite(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, w);
        break;

    case ArgNone:
        return;
    }
}

void encodeImm(Argument *arg, FILE *w, uint8_t sz)
{

    fwrite(&arg->Imm, 8, 1, w); // TODO
}

int encodeLD(Instruction *ins, FILE *w)
{

    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    fwrite(&byte, 1, 1, w);

    encodeCommon(&ins->Arg2, w);
    return 0;
}

int encodeST(Instruction *ins, FILE *w)
{

    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    fwrite(&byte, 1, 1, w);

    encodeCommon(&ins->Arg2, w);
    return 0;
}

int encodeMOV(Instruction *ins, FILE *w)
{

    if (ins->Arg2.Type == ArgRegister)
    {

        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        fwrite(&byte, 1, 1, w);
        return 0;
    }

    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4);
    fwrite(&byte, 1, 1, w);

    encodeImm(&ins->Arg2, w, ins->DataSz);
    return 0;
}

int encodePUSH(Instruction *ins, FILE *w)
{

    if (ins->Arg1.Type == ArgRegister)
    {

        fwrite(&ins->Arg1.RegNum, 1, 1, w);
        return 0;
    }

    encodeImm(&ins->Arg1, w, ins->DataSz);
    return 0;
}

int encodePOP(Instruction *ins, FILE *w)
{

    fwrite(&ins->Arg1.RegNum, 1, 1, w);

    return 0;
}

int encodeARITHM(Instruction *ins, FILE *w)
{

    if (ins->Arg2.Type == ArgRegister)
    {

        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        fwrite(&byte, 1, 1, w);
        return 0;
    }

    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4);
    fwrite(&byte, 1, 1, w);

    encodeImm(&ins->Arg2, w, ins->DataSz);
    return 0;
}

int encodeARITHMF(Instruction *ins, FILE *w)
{

    if (ins->Arg2.Type == ArgRegister)
    {

        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        fwrite(&byte, 1, 1, w);
        return 0;
    }

    uint8_t byte = ins->Arg1.RegNum;
    fwrite(&byte, 1, 1, w);

    fwrite(&ins->Arg2.Imm, sizeof(double), 1, w);
    return 0;
}

int encodeJMP(Instruction *ins, FILE *w)
{

    if (ins->Arg1.Type == ArgImm)
    {

        encodeImm(&ins->Arg1, w, ins->DataSz);
        return 0;
    }

    encodeCommon(&ins->Arg1, w);

    return 0;
}
