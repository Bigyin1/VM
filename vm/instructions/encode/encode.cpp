#include <ctype.h>
#include <cstddef>
#include "encode.hpp"

static size_t my_fwrite(const void *__ptr, size_t __size, size_t __nitems, FILE *__stream, bool evalSz)
{
    if (evalSz)
        return __size * __nitems;

    return fwrite(__ptr, __size, __nitems, __stream);
}

static size_t encodeNoArgs(Instruction *, FILE *, bool)
{
    return 1;
}

static size_t encode_ret(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeNoArgs(ins, w, evalSz);
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

static size_t encode_ld(Instruction *ins, FILE *w, bool evalSz)
{
    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz);
}

static size_t encode_st(Instruction *ins, FILE *w, bool evalSz)
{

    size_t sz = 1;
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4); // sign extend ???
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz);
}

static size_t encode_mov(Instruction *ins, FILE *w, bool evalSz)
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

static size_t encode_push(Instruction *ins, FILE *w, bool evalSz)
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

static size_t encode_pop(Instruction *ins, FILE *w, bool evalSz)
{
    uint8_t byte = ins->Arg1.RegNum | (ins->DataSz << 4) | (ins->SignExtend << 6);
    return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
}

static size_t encodeARITHM(Instruction *ins, FILE *w, bool evalSz)
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

static size_t encodeARITHMF(Instruction *ins, FILE *w, bool evalSz)
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

static size_t encode_add(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHM(ins, w, evalSz);
}

static size_t encode_addf(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHMF(ins, w, evalSz);
}

static size_t encode_sub(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHM(ins, w, evalSz);
}

static size_t encode_subf(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHMF(ins, w, evalSz);
}

static size_t encode_mul(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHM(ins, w, evalSz);
}

static size_t encode_mulf(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHMF(ins, w, evalSz);
}

static size_t encode_div(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHM(ins, w, evalSz);
}

static size_t encode_divf(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHMF(ins, w, evalSz);
}

static size_t encode_jmp(Instruction *ins, FILE *w, bool evalSz)
{
    size_t sz = 1;
    uint8_t byte = ins->JmpType;
    sz += my_fwrite(&byte, 1, 1, w, evalSz);
    ins->Arg1._immArgSz = DataWord; // encoded as 8 bytes for now;

    return sz + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz);
}

static size_t encode_call(Instruction *ins, FILE *w, bool evalSz)
{
    ins->Arg1._immArgSz = DataWord; // encoded as 8 bytes for now;

    return 1 + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz);
}

static size_t encode_cmp(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeARITHM(ins, w, evalSz);
}

static size_t encode_halt(Instruction *ins, FILE *w, bool evalSz)
{
    return encodeNoArgs(ins, w, evalSz);
}

static const uint8_t opCodeMask = 0b00011111;

static uint8_t encInstrHeader(uint8_t opCode, uint8_t argSetIdx)
{

    uint8_t byte = 0;

    byte |= (opCode & opCodeMask);
    byte |= (uint8_t)(argSetIdx << 5);

    return byte;
}

typedef size_t (*EncFunc)(Instruction *, FILE *, bool);

static EncFunc getEncFunc(InstrOpCode opCode)
{

    switch (opCode)
    {

#define INSTR(name, opCode, argSets) \
    case opCode:                     \
        return encode_##name;

#include "instructionsMeta.inc"

#undef INSTR

    default:
        return NULL;
    }
}

int Encode(Instruction *ins, FILE *w)
{

    uint8_t byte1 = encInstrHeader(ins->im->OpCode, ins->ArgSetIdx);
    fwrite(&byte1, 1, 1, w);

    getEncFunc(ins->im->OpCode)(ins, w, false);
    return 0;
}

size_t EvalInstrSize(Instruction *ins)
{

    EncFunc func = getEncFunc(ins->im->OpCode);

    if (func)
        return func(ins, NULL, true);

    return 0;
}
