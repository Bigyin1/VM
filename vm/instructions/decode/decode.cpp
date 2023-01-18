#include <assert.h>
#include "decode.hpp"
#include "opcodes.hpp"

static uint8_t regCodeMask = 0b00001111;

static InstrEncDecErr decodeCommon(Argument *arg, FILE *r)
{

    switch (arg->Type)
    {
    case ArgRegister:
        if (fread(&arg->RegNum, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgImm:
        if (fread(&arg->Imm, DataSzToBytesSz(arg->_immArgSz), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgRegisterIndirect:
        if (fread(&arg->RegNum, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgImmIndirect:
        if (fread(&arg->Imm, DataSzToBytesSz(arg->_immArgSz), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgRegisterOffsetIndirect:
        if (fread(&arg->RegNum, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;
        if (fread(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgImmOffsetIndirect:
        if (fread(&arg->Imm, DataSzToBytesSz(arg->_immArgSz), 1, r) == 0)
            return INSTR_NOT_EXIST;
        if (fread(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgNone:
        return INSTR_OK;
    }

    return INSTR_OK;
}

static InstrEncDecErr decode_ld(Instruction *ins, FILE *r)
{

    // arg 1
    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    ins->Arg1.RegNum = byte & regCodeMask;
    ins->DataSz = (DataSize)((byte & 0b00110000) >> 4);
    ins->SignExtend = (byte & 0b01000000) >> 6;

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

static InstrEncDecErr decode_st(Instruction *ins, FILE *r)
{

    // arg 1
    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    ins->Arg1.RegNum = byte & regCodeMask;
    ins->DataSz = (DataSize)((byte & 0b00110000) >> 4);

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

static InstrEncDecErr decode_mov(Instruction *ins, FILE *r)
{

    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    // mov r1, r2
    if (ins->Arg2.Type == ArgRegister)
    {

        ins->Arg2.RegNum = byte >> 4;
        ins->Arg1.RegNum = byte & regCodeMask;
        return INSTR_OK;
    }

    // arg 1
    ins->Arg1.RegNum = byte & regCodeMask;
    ins->Arg2._immArgSz = (DataSize)((byte & 0b00110000) >> 4);
    ins->SignExtend = (byte & 0b01000000) >> 6;

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

static InstrEncDecErr decode_push(Instruction *ins, FILE *r)
{

    if (ins->Arg1.Type == ArgImm)
    {
        uint8_t byte = 0;
        if (fread(&byte, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;

        ins->DataSz = (DataSize)(byte & 0b00000011);
        ins->Arg1._immArgSz = (DataSize)((byte & 0b00001100) >> 2);

        if (decodeCommon(&ins->Arg1, r) != INSTR_OK)
            return INSTR_NOT_EXIST;

        return INSTR_OK;
    }

    if (ins->Arg1.Type == ArgRegister)
    {
        ins->DataSz = (DataSize)((ins->Arg1.RegNum & 0b00110000) >> 4);
        ins->Arg1.RegNum &= regCodeMask;
        return decodeCommon(&ins->Arg1, r);
    }
    return INSTR_OK;
}

static InstrEncDecErr decode_pop(Instruction *ins, FILE *r)
{

    if (decodeCommon(&ins->Arg1, r) != INSTR_OK)
        return INSTR_NOT_EXIST;

    ins->DataSz = (DataSize)((ins->Arg1.RegNum & 0b00110000) >> 4);
    ins->SignExtend = (ins->Arg1.RegNum & 0b01000000) >> 6;
    ins->Arg1.RegNum &= regCodeMask;

    return INSTR_OK;
}

static InstrEncDecErr decodeARITHM(Instruction *ins, FILE *r)
{

    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    // add r1, r2
    if (ins->Arg2.Type == ArgRegister)
    {

        ins->Arg2.RegNum = byte >> 4;
        ins->Arg1.RegNum = byte & regCodeMask;
        return INSTR_OK;
    }

    // arg 1
    ins->Arg1.RegNum = byte & regCodeMask;
    ins->Arg2._immArgSz = (DataSize)(byte >> 4);

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

static InstrEncDecErr decodeARITHMF(Instruction *ins, FILE *r)
{

    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    // addf r1, r2
    if (ins->Arg2.Type == ArgRegister)
    {

        ins->Arg2.RegNum = byte >> 4;
        ins->Arg1.RegNum = byte & regCodeMask;
        return INSTR_OK;
    }

    // arg 1
    ins->Arg1.RegNum = byte;

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

static InstrEncDecErr decode_add(Instruction *ins, FILE *r)
{

    return decodeARITHM(ins, r);
}

static InstrEncDecErr decode_addf(Instruction *ins, FILE *r)
{

    return decodeARITHMF(ins, r);
}

static InstrEncDecErr decode_sub(Instruction *ins, FILE *r)
{

    return decodeARITHM(ins, r);
}

static InstrEncDecErr decode_subf(Instruction *ins, FILE *r)
{

    return decodeARITHMF(ins, r);
}

static InstrEncDecErr decode_mul(Instruction *ins, FILE *r)
{

    return decodeARITHM(ins, r);
}

static InstrEncDecErr decode_mulf(Instruction *ins, FILE *r)
{

    return decodeARITHMF(ins, r);
}

static InstrEncDecErr decode_div(Instruction *ins, FILE *r)
{

    return decodeARITHM(ins, r);
}

static InstrEncDecErr decode_divf(Instruction *ins, FILE *r)
{

    return decodeARITHMF(ins, r);
}

static InstrEncDecErr decode_jmp(Instruction *ins, FILE *r)
{
    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    ins->JmpType = (JumpType)byte;

    ins->Arg1._immArgSz = DataWord;

    return decodeCommon(&ins->Arg1, r);
}

static InstrEncDecErr decode_call(Instruction *ins, FILE *r)
{

    ins->Arg1._immArgSz = DataWord;

    return decodeCommon(&ins->Arg1, r);
}

static InstrEncDecErr decode_cmp(Instruction *ins, FILE *r)
{

    return decodeARITHM(ins, r);
}

static InstrEncDecErr decodeNoArgs(Instruction *, FILE *)
{
    return INSTR_OK;
}

static InstrEncDecErr decode_ret(Instruction *i, FILE *r)
{
    return decodeNoArgs(i, r);
}

static InstrEncDecErr decode_halt(Instruction *i, FILE *r)
{
    return decodeNoArgs(i, r);
}

typedef InstrEncDecErr (*DecFunc)(Instruction *, FILE *);

DecFunc getDecoder(InstrOpCode opCode)
{

    switch (opCode)
    {

#define INSTR(name, opCode, argSets) \
    case opCode:                     \
        return decode_##name;

#include "instructionsMeta.inc"

#undef INSTR

    default:
        break;
    }

    return NULL;
}

static const uint8_t opCodeMask = 0b00011111;

static InstrEncDecErr newInstructionFromOpCode(Instruction *ins, InstrOpCode opCode, uint8_t argSetIdx)
{
    assert(ins != NULL);

    ins->im = FindInsMetaByOpCode(opCode);
    if (ins->im == NULL)
        return INSTR_UNKNOWN;

    if (ins->ArgSetIdx != 0 && ins->im->ArgSets[argSetIdx].First == ArgNone)
        return INSTR_WRONG_OPERANDS;

    ins->ArgSetIdx = argSetIdx;

    ins->Arg1.Type = ins->im->ArgSets[argSetIdx].First;
    ins->Arg2.Type = ins->im->ArgSets[argSetIdx].Second;

    return INSTR_OK;
}

InstrEncDecErr Decode(Instruction *ins, FILE *r)
{
    assert(ins != NULL);
    assert(r != NULL);

    char byte = 0;

    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    InstrOpCode opCode = (InstrOpCode)(byte & opCodeMask);
    uint8_t argSetIdx = (uint8_t)(byte >> 5);

    InstrEncDecErr err = newInstructionFromOpCode(ins, opCode, argSetIdx);
    if (err != INSTR_OK)
        return err;

    return getDecoder(opCode)(ins, r);
}