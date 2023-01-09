#include <ctype.h>
#include <cstddef>
#include "encode.hpp"

static size_t my_fwrite(const void *__ptr, size_t __size, size_t __nitems, FILE *__stream, bool evalSz)
{
    if (evalSz)
        return __size * __nitems;

    return fwrite(__ptr, __size, __nitems, __stream);
}

static size_t encodeCommon(Argument *arg, FILE *w, DataSize argSz, bool evalSz)
{

    switch (arg->Type)
    {
    case ArgRegister:
        return my_fwrite(&arg->RegNum, 1, 1, w, evalSz);

    case ArgImm:
        return my_fwrite(&arg->Imm, DataSzToBytesSz(argSz), 1, w, evalSz);

    case ArgRegisterIndirect:
        return my_fwrite(&arg->RegNum, 1, 1, w, evalSz);

    case ArgImmIndirect:

        return my_fwrite(&arg->Imm, sizeof(arg->Imm), 1, w, evalSz);

    case ArgRegisterOffsetIndirect:
        return my_fwrite(&arg->RegNum, 1, 1, w, evalSz) +
               my_fwrite(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, w, evalSz);

    case ArgImmOffsetIndirect:

        return my_fwrite(&arg->Imm, sizeof(arg->Imm), 1, w, evalSz) +
               my_fwrite(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, w, evalSz);

    case ArgNone:
        return 0;
    }
}

size_t encodeLD(Instruction *ins, FILE *w, bool evalSz)
{
    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz);
}

size_t encodeST(Instruction *ins, FILE *w, bool evalSz)
{

    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4); // sign extend ???
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz);
}

size_t encodeMOV(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg2.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;

    uint8_t byte = ins->Arg1.RegNum | (ins->Arg2._immArgSz << 4) | (ins->SignExtend << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz);
}

size_t encodePUSH(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg1.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4);

        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;

    uint8_t byte = (ins->DataSz) | (ins->Arg1._immArgSz << 2); // sign extend ???
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz);
}

size_t encodePOP(Instruction *ins, FILE *w, bool evalSz)
{
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
}

size_t encodeARITHM(Instruction *ins, FILE *w, bool evalSz)
{

    if (ins->Arg2.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (ins->Arg2.RegNum << 4);
        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->Arg2._immArgSz << 4);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz);
}

size_t encodeARITHMF(Instruction *ins, FILE *w, bool evalSz)
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

size_t encodeJMP(Instruction *ins, FILE *w, bool evalSz)
{
    size_t sz = 1;
    uint8_t byte = ins->JmpType;
    sz += my_fwrite(&byte, 1, 1, w, evalSz);
    ins->Arg1._immArgSz = DataWord; // encoded as 8 bytes for now;

    return sz + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz);
}

size_t encodeCALL(Instruction *ins, FILE *w, bool evalSz)
{
    ins->Arg1._immArgSz = DataWord; // encoded as 8 bytes for now;

    return 1 + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz);
}

size_t encodeNoArgs(Instruction *, FILE *, bool)
{
    return 1;
}
