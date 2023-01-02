#include "decode.hpp"

static uint8_t regCodeMask = 0b00001111;

static void decodeCommon(Argument *arg, FILE *r)
{

    switch (arg->Type)
    {
    case ArgRegister:
        fread(&arg->RegNum, 1, 1, r);
        break;

    case ArgImm:
        fread(&arg->Imm, sizeof(arg->Imm), 1, r);
        break;

    case ArgRegisterIndirect:
        fread(&arg->RegNum, 1, 1, r);
        break;

    case ArgImmIndirect:
        fread(&arg->Imm, sizeof(arg->Imm), 1, r);
        break;

    case ArgRegisterOffsetIndirect:
        fread(&arg->RegNum, 1, 1, r);
        fread(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, r);
        break;

    case ArgImmOffsetIndirect:
        fread(&arg->Imm, sizeof(arg->Imm), 1, r);
        fread(&arg->ImmDisp16, sizeof(arg->ImmDisp16), 1, r);
        break;

    case ArgNone:
        return;
    }
}

int decodeLD(Instruction *ins, FILE *r)
{

    // arg 1
    uint8_t byte = 0;
    fread(&byte, 1, 1, r);

    ins->Arg1.RegNum = byte & regCodeMask;
    ins->DataSz = (byte & 0b00110000) >> 4;
    ins->SignExtend = (byte & 0b01000000) >> 6;

    // arg 2
    decodeCommon(&ins->Arg2, r);

    return 0;
}

int decodeST(Instruction *ins, FILE *r)
{

    // arg 1
    uint8_t byte = 0;
    fread(&byte, 1, 1, r);

    ins->Arg1.RegNum = byte & regCodeMask;
    ins->DataSz = (byte & 0b00110000) >> 4;

    // arg 2
    decodeCommon(&ins->Arg2, r);

    return 0;
}

int decodeMOV(Instruction *ins, FILE *r)
{

    uint8_t byte = 0;
    fread(&byte, 1, 1, r);

    // mov r1, r2
    if (ins->Arg2.Type == ArgRegister)
    {

        ins->Arg2.RegNum = byte >> 4;
        ins->Arg1.RegNum = byte & regCodeMask;
        return 0;
    }

    // arg 1
    ins->Arg1.RegNum = byte & regCodeMask; // TODO data sz

    // arg 2
    decodeCommon(&ins->Arg2, r);

    return 0;
}

int decodePUSH(Instruction *ins, FILE *r)
{
    decodeCommon(&ins->Arg1, r);
    if (ins->Arg1.Type == ArgRegister)
    {
        ins->DataSz = (ins->Arg1.RegNum & 0b00110000) >> 4;
        ins->Arg1.RegNum &= regCodeMask;
    }
    return 0;
}

int decodePOP(Instruction *ins, FILE *r)
{

    decodeCommon(&ins->Arg1, r);

    ins->DataSz = (ins->Arg1.RegNum & 0b00110000) >> 4;
    ins->SignExtend = (ins->Arg1.RegNum & 0b01000000) >> 6;
    ins->Arg1.RegNum &= regCodeMask;

    return 0;
}

int decodeARITHM(Instruction *ins, FILE *r)
{

    uint8_t byte = 0;
    fread(&byte, 1, 1, r);

    // add r1, r2
    if (ins->Arg2.Type == ArgRegister)
    {

        ins->Arg2.RegNum = byte >> 4;
        ins->Arg1.RegNum = byte & regCodeMask;
        return 0;
    }

    // arg 1
    ins->Arg1.RegNum = byte & regCodeMask; // TODO data sz

    // arg 2
    decodeCommon(&ins->Arg2, r);

    return 0;
}

int decodeARITHMF(Instruction *ins, FILE *r)
{

    uint8_t byte = 0;
    fread(&byte, 1, 1, r);

    // addf r1, r2
    if (ins->Arg2.Type == ArgRegister)
    {

        ins->Arg2.RegNum = byte >> 4;
        ins->Arg1.RegNum = byte & regCodeMask;
        return 0;
    }

    // arg 1
    ins->Arg1.RegNum = byte;

    // arg 2
    decodeCommon(&ins->Arg2, r);

    return 0;
}

int decodeBranch(Instruction *ins, FILE *r)
{

    decodeCommon(&ins->Arg1, r);
    return 0;
}

int decodeNoArgs(Instruction *, FILE *)
{
    return 0;
}
