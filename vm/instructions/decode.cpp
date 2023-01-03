#include "decode.hpp"

static uint8_t regCodeMask = 0b00001111;

static InstrErr decodeCommon(Argument *arg, FILE *r)
{

    switch (arg->Type)
    {
    case ArgRegister:
        if (fread(&arg->RegNum, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgImm:
        if (fread(&arg->Imm, sizeof(arg->Imm), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgRegisterIndirect:
        if (fread(&arg->RegNum, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgImmIndirect:
        if (fread(&arg->Imm, sizeof(arg->Imm), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgRegisterOffsetIndirect:
        if (fread(&arg->RegNum, 1, 1, r) == 0)
            return INSTR_NOT_EXIST;
        if (fread(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgImmOffsetIndirect:
        if (fread(&arg->Imm, sizeof(arg->Imm), 1, r) == 0)
            return INSTR_NOT_EXIST;
        if (fread(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, r) == 0)
            return INSTR_NOT_EXIST;
        break;

    case ArgNone:
        return INSTR_OK;
    }

    return INSTR_OK;
}

InstrErr decodeLD(Instruction *ins, FILE *r)
{

    // arg 1
    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    ins->Arg1.RegNum = byte & regCodeMask;
    ins->DataSz = (byte & 0b00110000) >> 4;
    ins->SignExtend = (byte & 0b01000000) >> 6;

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

InstrErr decodeST(Instruction *ins, FILE *r)
{

    // arg 1
    uint8_t byte = 0;
    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    ins->Arg1.RegNum = byte & regCodeMask;
    ins->DataSz = (byte & 0b00110000) >> 4;

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

InstrErr decodeMOV(Instruction *ins, FILE *r)
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
    ins->Arg1.RegNum = byte & regCodeMask; // TODO data sz

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

InstrErr decodePUSH(Instruction *ins, FILE *r)
{
    if (decodeCommon(&ins->Arg1, r) != INSTR_OK)
        return INSTR_NOT_EXIST;

    if (ins->Arg1.Type == ArgRegister)
    {
        ins->DataSz = (ins->Arg1.RegNum & 0b00110000) >> 4;
        ins->Arg1.RegNum &= regCodeMask;
    }
    return INSTR_OK;
}

InstrErr decodePOP(Instruction *ins, FILE *r)
{

    if (decodeCommon(&ins->Arg1, r) != INSTR_OK)
        return INSTR_NOT_EXIST;

    ins->DataSz = (ins->Arg1.RegNum & 0b00110000) >> 4;
    ins->SignExtend = (ins->Arg1.RegNum & 0b01000000) >> 6;
    ins->Arg1.RegNum &= regCodeMask;

    return INSTR_OK;
}

InstrErr decodeARITHM(Instruction *ins, FILE *r)
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
    ins->Arg1.RegNum = byte & regCodeMask; // TODO data sz

    // arg 2
    return decodeCommon(&ins->Arg2, r);
}

InstrErr decodeARITHMF(Instruction *ins, FILE *r)
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

InstrErr decodeBranch(Instruction *ins, FILE *r)
{

    return decodeCommon(&ins->Arg1, r);
}

InstrErr decodeNoArgs(Instruction *, FILE *)
{
    return INSTR_OK;
}
