#include "encode.hpp"

#include <ctype.h>

#include <cstddef>

static size_t my_fwrite(const void* __ptr, size_t __size, size_t __nitems, FILE* __stream,
                        bool evalSz)
{
    if (evalSz)
        return __size * __nitems;

    return fwrite(__ptr, __size, __nitems, __stream);
}

static size_t encodeNoArgs(Instruction*, FILE*, bool, bool) { return 1; }

static size_t encode_ret(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeNoArgs(ins, w, evalSz, evalSymOffset);
}

static size_t encodeCommon(Argument* arg, FILE* w, DataSize argSz, bool evalSz, bool)
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

        case ArgNone:
            return 0;

        default:
            return 0;
    }
}

static size_t encode_ld(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    size_t  sz   = 1;
    uint8_t byte = ins->Arg1.RegNum | (uint8_t)(ins->DataSz << 4) | (uint8_t)(ins->SignExt << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_st(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{

    size_t  sz   = 1;
    uint8_t byte = ins->Arg1.RegNum | (uint8_t)(ins->DataSz << 4);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_mov(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{

    if (ins->Arg2.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (uint8_t)(ins->Arg2.RegNum << 4);
        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;

    uint8_t byte =
        ins->Arg1.RegNum | (uint8_t)(ins->Arg2._immArgSz << 4) | (uint8_t)(ins->SignExt << 6);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_push(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{

    if (ins->Arg1.Type == ArgRegister)
    {
        uint8_t byte = ins->Arg1.RegNum | (uint8_t)(ins->DataSz << 4);

        return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
    }

    size_t sz = 1;

    uint8_t byte = (uint8_t)(ins->DataSz) | (uint8_t)(ins->Arg1._immArgSz << 2);
    sz += my_fwrite(&byte, 1, 1, w, evalSz);

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_pop(Instruction* ins, FILE* w, bool evalSz, bool)
{
    uint8_t byte = ins->Arg1.RegNum | (uint8_t)(ins->DataSz << 4) | (uint8_t)(ins->SignExt << 6);
    return 1 + my_fwrite(&byte, 1, 1, w, evalSz);
}

static size_t encodeARITHM(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    size_t sz = 1;

    uint8_t byte = 0;
    if (ins->Arg2.Type != ArgImm)
        byte = ins->Arg1.RegNum | (uint8_t)(ins->Arg2.RegNum << 4);
    else
        byte = ins->Arg1.RegNum | (uint8_t)(ins->Arg2._immArgSz << 4);

    sz += my_fwrite(&byte, 1, 1, w, evalSz);
    if (ins->Arg2.Type == ArgRegister || ins->Arg2.Type == ArgRegisterIndirect)
        return sz;

    if (ins->Arg2.Type == ArgRegisterOffsetIndirect)
    {
        sz += my_fwrite(&ins->Arg2.ImmDisp16, sizeof(ins->Arg2.ImmDisp16), 1, w, evalSz);
        return sz;
    }

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg2, w, ins->Arg2._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_add(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_addf(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_sub(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_subf(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_mul(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_mulf(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_div(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_divf(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_sqrt(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_cmp(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_cmpf(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeARITHM(ins, w, evalSz, evalSymOffset);
}

static size_t encode_jmp(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    size_t  sz   = 1;
    uint8_t byte = ins->JmpType;
    sz += my_fwrite(&byte, 1, 1, w, evalSz);
    ins->Arg1._immArgSz = DataWord; // address encoded as 8 bytes for now;

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_call(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    ins->Arg1._immArgSz = DataWord; // address encoded as 8 bytes for now;
    size_t sz           = 1;

    if (evalSymOffset)
        return sz;

    return sz + encodeCommon(&ins->Arg1, w, ins->Arg1._immArgSz, evalSz, evalSymOffset);
}

static size_t encode_halt(Instruction* ins, FILE* w, bool evalSz, bool evalSymOffset)
{
    return encodeNoArgs(ins, w, evalSz, evalSymOffset);
}

static const uint8_t opCodeMask = 0b00011111;

static uint8_t encInstrHeader(uint8_t opCode, uint8_t argSetIdx)
{

    uint8_t byte = 0;

    byte |= (opCode & opCodeMask);
    byte |= (uint8_t)(argSetIdx << 5);

    return byte;
}

typedef size_t (*EncFunc)(Instruction*, FILE*, bool, bool);

static EncFunc getEncFunc(InstrOpCode opCode)
{

    switch (opCode)
    {

#define INSTR(name, opCode, argSets)                                                               \
    case opCode:                                                                                   \
        return encode_##name;

#include "instructionsMeta.inc"

#undef INSTR

        default:
            return NULL;
    }
}

int Encode(Instruction* ins, FILE* w)
{
    uint8_t byte1 = encInstrHeader(ins->im->OpCode, ins->ArgSetIdx);
    fwrite(&byte1, 1, 1, w);

    getEncFunc(ins->im->OpCode)(ins, w, false, false);

    return ferror(w);
}

size_t EvalInstrSize(Instruction* ins)
{

    EncFunc func = getEncFunc(ins->im->OpCode);

    if (func != NULL)
        return func(ins, NULL, true, false);

    return 0;
}

size_t EvalInstrSymbolOffset(Instruction* ins)
{

    EncFunc func = getEncFunc(ins->im->OpCode);

    if (func != NULL)
        return func(ins, NULL, true, true);

    return 0;
}
