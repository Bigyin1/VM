#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "instructions.hpp"
#include "encode.hpp"
#include "decode.hpp"

const RegMeta regs[] = {

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
        .RegCode = 13,
    }};

const InstructionMeta instructions[] = {
    {
        .Name = "ret",
        .OpCode = RET,
        .ArgSets = {
            {.First = ArgNone, .Second = ArgNone},
        },
        .encFunc = encodeNoArgs,
        .decFunc = decodeNoArgs,
    },
    {
        .Name = "ld",
        .OpCode = LD,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegisterIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
            {.First = ArgRegister, .Second = ArgImmIndirect},
            {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        },
        .encFunc = encodeLD,
        .decFunc = decodeLD,

    },
    {
        .Name = "st",
        .OpCode = ST,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegisterIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
            {.First = ArgRegister, .Second = ArgImmIndirect},
            {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        },
        .encFunc = encodeST,
        .decFunc = decodeST,
    },
    {
        .Name = "mov",
        .OpCode = MOV,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeMOV,
        .decFunc = decodeMOV,
    },
    {
        .Name = "push",
        .OpCode = PUSH,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgImm, .Second = ArgNone},
        },
        .encFunc = encodePUSH,
        .decFunc = decodePUSH,
    },
    {
        .Name = "pop",
        .OpCode = POP,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
        },
        .encFunc = encodePOP,
        .decFunc = decodePOP,
    },
    {
        .Name = "add",
        .OpCode = ADD,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "addf",
        .OpCode = ADDF,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "sub",
        .OpCode = SUB,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "subf",
        .OpCode = SUBF,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "mul",
        .OpCode = MUL,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "mulf",
        .OpCode = MULF,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "div",
        .OpCode = DIV,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHM,
        .decFunc = decodeARITHM,
    },
    {
        .Name = "divf",
        .OpCode = DIVF,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeARITHMF,
        .decFunc = decodeARITHMF,
    },
    {
        .Name = "jmp",
        .OpCode = JMP,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
    },
    {
        .Name = "jeq",
        .OpCode = JEQ,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
    },
    {
        .Name = "jneq",
        .OpCode = JNEQ,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
    },
    {
        .Name = "jg",
        .OpCode = JG,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
    },
    {
        .Name = "jl",
        .OpCode = JL,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
    },
    {
        .Name = "call",
        .OpCode = CALL,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
    },
    {
        .Name = "cmp",
        .OpCode = CMP,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .encFunc = encodeMOV,
        .decFunc = decodeMOV,
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
            for (size_t j = 0; j < sizeof(instructions->ArgSets) / sizeof(ArgSet); j++)
            {
                if (instructions[i].ArgSets[j].First == args.First &&
                    instructions[i].ArgSets[j].Second == args.Second)

                    return j;
            }
        }
    }

    return -1;
}

int FindRegByName(RegName name)
{

    for (size_t i = 0; i < sizeof(regs) / sizeof(RegMeta); i++)
    {
        if (strcmp(regs[i].Name, name) == 0)
            return regs[i].RegCode;
    }

    return -1;
}

static InstrErr newInstructionFromOpCode(Instruction *ins, uint8_t opCode, uint8_t argSetIdx)
{
    assert(ins != NULL);

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

InstrErr Decode(Instruction *ins, FILE *r)
{
    assert(ins != NULL);
    assert(r != NULL);

    char byte = 0;

    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    uint8_t opCode = (byte & opCodeMask);
    uint8_t argSetIdx = (uint8_t)(byte >> 5);

    InstrErr err = newInstructionFromOpCode(ins, opCode, argSetIdx);
    if (err != INSTR_OK)
        return err;

    return ins->im->decFunc(ins, r);
}

static uint8_t encInstrHeader(uint8_t opCode, uint8_t argSetIdx)
{

    uint8_t byte = 0;

    byte |= (opCode & opCodeMask);
    byte |= (uint8_t)(argSetIdx << 5);

    return byte;
}

int Encode(Instruction *ins, FILE *w)
{

    uint8_t byte1 = encInstrHeader(ins->im->OpCode, ins->ArgSetIdx);
    fwrite(&byte1, 1, 1, w);

    ins->im->encFunc(ins, w, false);
    return 0;
}

InstrErr NewInstruction(InstructionName name, Instruction *instr, size_t *sz)
{

    const InstructionMeta *im = findInsMetaByName(name);
    if (im == NULL)
        return INSTR_UNKNOWN;

    instr->im = im;

    ArgSet argSet = {.First = instr->Arg1.Type, .Second = instr->Arg2.Type};

    int argSetIdx = findArgSetIdx(im->OpCode, argSet);
    if (argSetIdx < 0)
        return INSTR_WRONG_OPERANDS;

    instr->ArgSetIdx = (uint8_t)argSetIdx;

    *sz = instr->im->encFunc(instr, NULL, true);
    return INSTR_OK;
}
