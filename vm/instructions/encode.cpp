#include <ctype.h>
#include <cstddef>
#include "encode.hpp"

static size_t my_fwrite(const void *__ptr, size_t __size, size_t __nitems, FILE *__stream, bool evalSz)
{
    if (evalSz)
        return __size * __nitems;

    return fwrite(__ptr, __size, __nitems, __stream);
}

static size_t encodeCommon(Argument *arg, FILE *w, bool evalSz)
{

    switch (arg->Type)
    {
    case ArgRegister:
        return my_fwrite(&arg->RegNum, 1, 1, w, evalSz);
        break;

    case ArgRegisterIndirect:
        return my_fwrite(&arg->RegNum, 1, 1, w, evalSz);
        break;

    case ArgImmIndirect:

        return my_fwrite(&arg->Imm, sizeof(arg->Imm), 1, w, evalSz);
        break;

    case ArgRegisterOffsetIndirect:
        return my_fwrite(&arg->RegNum, 1, 1, w, evalSz) +
               my_fwrite(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, w, evalSz);
        break;

    case ArgImmOffsetIndirect:

        return my_fwrite(&arg->Imm, sizeof(arg->Imm), 1, w, evalSz) +
               my_fwrite(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, w, evalSz);
        break;

    case ArgNone:
        return 0;
    }
}

static size_t encodeImm(Argument *arg, FILE *w, uint8_t sz, bool evalSz)
{
    return my_fwrite(&arg->Imm, 8, 1, w, evalSz); // TODO
}

int encodeLD(Instruction *ins, FILE *w, bool evalSz)
{
    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, evalSz);
}

int encodeST(Instruction *ins, FILE *w, bool evalSz)
{

    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, evalSz);
}

int encodeMOV(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg2.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;

    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeImm(&ins->Arg2, w, ins->DataSz, evalSz);
}

int encodePUSH(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg1.Type == ArgRegister)
        return 1 + my_fwrite(&ins->Arg1.RegNum, 1, 1, w, evalSz);

    return 1 + encodeImm(&ins->Arg1, w, ins->DataSz, evalSz);
}

int encodePOP(Instruction *ins, FILE *w, bool evalSz)
{
    return 1 + my_fwrite(&ins->Arg1.RegNum, 1, 1, w, evalSz);
}

int encodeARITHM(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg2.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeImm(&ins->Arg2, w, ins->DataSz, evalSz);
}

int encodeARITHMF(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg2.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum;
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + my_fwrite(&ins->Arg2.Imm, sizeof(double), 1, w, evalSz);
}

int encodeJMP(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg1.Type == ArgImm)
        return 1 + encodeImm(&ins->Arg1, w, ins->DataSz, evalSz);

    return 1 + encodeCommon(&ins->Arg1, w, evalSz);
}
