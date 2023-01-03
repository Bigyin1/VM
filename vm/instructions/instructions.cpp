#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "instructions.hpp"
#include "encode.hpp"
#include "decode.hpp"
#include "run.hpp"

const RegMeta regs[] = {

    {
        .Name = "r0",
        .RegCode = R0,
    },
    {
        .Name = "r1",
        .RegCode = R1,
    },
    {
        .Name = "r2",
        .RegCode = R2,
    },
    {
        .Name = "r3",
        .RegCode = R3,
    },
    {
        .Name = "r4",
        .RegCode = R4,
    },
    {
        .Name = "r5",
        .RegCode = R5,
    },
    {
        .Name = "rbp",
        .RegCode = RBP,
    },
    {
        .Name = "rsp",
        .RegCode = RSP,
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
        .runFunc = runRET,
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
        .runFunc = runLD,

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
        .runFunc = runST,
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
        .runFunc = runMOV,
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
        .runFunc = runPUSH,
    },
    {
        .Name = "pop",
        .OpCode = POP,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
        },
        .encFunc = encodePOP,
        .decFunc = decodePOP,
        .runFunc = runPOP,
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
        .runFunc = runADD,
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
        .runFunc = runADDF,
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
        .runFunc = runSUB,
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
        .runFunc = runSUBF,
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
        .runFunc = runMUL,
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
        .runFunc = runMULF,
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
        .runFunc = runDIV,
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
        .runFunc = runDIVF,
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
        .runFunc = runJMP,
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
        .runFunc = runJEQ,
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
        .runFunc = runJNEQ,
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
        .runFunc = runJG,
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
        .runFunc = runJL,
    },
    {
        .Name = "call",
        .OpCode = CALL,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone}, {.First = ArgRegister, .Second = ArgNone},
            // {.First = ArgRegisterIndirect, .Second = ArgNone},
            // {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .encFunc = encodeBranch,
        .decFunc = decodeBranch,
        .runFunc = runCALL,
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
        .runFunc = runCMP,
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

static InstrDecErr newInstructionFromOpCode(Instruction *ins, uint8_t opCode, uint8_t argSetIdx)
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

InstrDecErr Decode(Instruction *ins, FILE *r)
{
    assert(ins != NULL);
    assert(r != NULL);

    char byte = 0;

    if (fread(&byte, 1, 1, r) == 0)
        return INSTR_NOT_EXIST;

    uint8_t opCode = (byte & opCodeMask);
    uint8_t argSetIdx = (uint8_t)(byte >> 5);

    InstrDecErr err = newInstructionFromOpCode(ins, opCode, argSetIdx);
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

InstrDecErr NewInstruction(InstructionName name, Instruction *instr, size_t *sz)
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
