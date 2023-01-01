#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "instructions.hpp"
#include "encode.hpp"
#include "decode.hpp"

const RegMeta regs[15] = {

    {
        .Name = "r0",
        .RegCode = 0,
    },
    {
        .Name = "r1",
        .RegCode = 1,
    },
    {
        .Name = "r2",
        .RegCode = 2,
    },
    {
        .Name = "r3",
        .RegCode = 3,
    },
    {
        .Name = "r4",
        .RegCode = 4,
    },
    {
        .Name = "r5",
        .RegCode = 5,
    },
    {
        .Name = "rbp",
        .RegCode = 14,
    }};

const InstructionMeta instructions[] = {

    {
        .Name = "ld",
        .OpCode = 1,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegisterIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetRegIndirect},
            {.First = ArgRegister, .Second = ArgImmIndirect},
            {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        },
        .encFunc = encodeLD,
        .decFunc = decodeLD,

    },
    {
        .Name = "st",
        .OpCode = 2,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegisterIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetRegIndirect},
            {.First = ArgRegister, .Second = ArgImmIndirect},
            {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        },
        .encFunc = encodeST,
        .decFunc = decodeST,
    },
    {
        .Name = "mov",
        .OpCode = 3,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeMOV,
        .decFunc = decodeMOV,
    },
    {
        .Name = "push",
        .OpCode = 4,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgImm, .Second = ArgNone},
        },
        .encFunc = encodePUSH,
        .decFunc = decodePUSH,
    },
    {
        .Name = "pop",
        .OpCode = 5,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
        },
        .encFunc = encodePOP,
        .decFunc = decodePOP,
    },
    {
        .Name = "add",
        .OpCode = 6,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "addf",
        .OpCode = 7,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "sub",
        .OpCode = 8,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "subf",
        .OpCode = 9,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "mul",
        .OpCode = 10,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "mulf",
        .OpCode = 11,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "div",
        .OpCode = 12,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "divf",
        .OpCode = 13,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "jmp",
        .OpCode = 14,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeJMP,
        .decFunc = decodeJMP,
    },

};

static const InstructionMeta *findInsMetaByName(InstructionName name)
{
    assert(name != NULL);

    for (size_t i = 0; i < sizeof(instructions) / sizeof(InstructionMeta); i++)
    {

        if (strcmp(instructions[i].Name, name) == 0)
            return &instructions[i];
    }

    return NULL;
}

static const InstructionMeta *findInsMetaByOpCode(uint8_t opCode)
{

    for (size_t i = 0; i < sizeof(instructions) / sizeof(InstructionMeta); i++)
    {

        if (opCode == instructions[i].OpCode)
            return &instructions[i];
    }

    return NULL;
}

static int findArgSetIdx(uint8_t opCode, ArgSet args)
{

    for (size_t i = 0; i < sizeof(instructions) / sizeof(InstructionMeta); i++)
    {

        if (opCode == instructions[i].OpCode)
        {
            for (size_t j = 0; i < sizeof(instructions->ArgSets) / sizeof(ArgSet); j++)
            {
                if (instructions[i].ArgSets[j].First == args.First &&
                    instructions[i].ArgSets[j].Second == args.Second)

                    return j;
            }
        }
    }

    return -1;
}

int findRegByName(RegName name)
{

    for (size_t i = 0; i < sizeof(regs) / sizeof(RegMeta); i++)
    {

        if (strcmp(regs[i].Name, name) == 0)
            return i;
    }

    return -1;
}

static InstrErr newInstructionFromOpCode(Instruction *ins, uint8_t opCode, uint8_t argSetIdx)
{

    ins->im = findInsMetaByOpCode(opCode);
    if (ins->im == NULL)
        return INSTR_UNKNOWN;

    if (ins->ArgSetIdx != 0 && ins->im->ArgSets[argSetIdx].First == ArgNone)
        return INSTR_WRONG_OPERANDS;

    ins->ArgSetIdx = argSetIdx;

    ins->Arg1.Type = ins->im->ArgSets[argSetIdx].First;
    ins->Arg2.Type = ins->im->ArgSets[argSetIdx].Second;

    return INSTR_OK;
}

static const uint8_t opCodeMask = 0b00011111;

int Decode(Instruction *ins, FILE *r)
{

    char byte = 0;

    fread(&byte, 1, 1, r);

    uint8_t opCode = (byte & opCodeMask);
    uint8_t argSetIdx = (byte >> 5);

    InstrErr err = newInstructionFromOpCode(ins, opCode, argSetIdx);
    if (err != INSTR_OK)
        return err;

    ins->im->decFunc(ins, r);

    return 0;
}

static uint8_t encInstrHeader(uint8_t opCode, uint8_t argSetIdx)
{

    uint8_t byte = 0;

    byte |= (opCode & opCodeMask);
    byte |= (argSetIdx << 5);

    return byte;
}

int Encode(Instruction *ins, FILE *w)
{

    uint8_t byte1 = encInstrHeader(ins->im->OpCode, ins->ArgSetIdx);
    fwrite(&byte1, 1, 1, w);

    ins->im->encFunc(ins, w);
    return 0;
}

InstrErr NewInstruction(InstructionName name, Instruction instr, size_t *sz)
{

    const InstructionMeta *im = findInsMetaByName(name);
    if (im == NULL)
        return INSTR_UNKNOWN;

    instr.im = im;

    ArgSet argSet = {.First = instr.Arg1.Type, .Second = instr.Arg2.Type};

    int argSetIdx = findArgSetIdx(im->OpCode, argSet);
    if (argSetIdx < 0)
        return INSTR_WRONG_OPERANDS;

    instr.ArgSetIdx = argSetIdx;
    return INSTR_OK;
}
