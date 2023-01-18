#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "instructions.hpp"

const InstructionMeta instructions[] = {

#define COMMA ,

#define INSTR(name, opCode, argSets)   \
    {                                  \
        .Name = #name,                 \
        .OpCode = InstrOpCode(opCode), \
        .ArgSets = argSets,            \
    },

#include "instructionsMeta.inc"

#undef INSTR
#undef COMMA

};

const InstructionMeta *FindInsMetaByName(InstructionName name)
{
    assert(name != NULL);

    for (size_t i = 0; i < sizeof(instructions) / sizeof(InstructionMeta); i++)
    {

        if (strcmp(instructions[i].Name, name) == 0)
            return &instructions[i];
    }

    return NULL;
}

const InstructionMeta *FindInsMetaByOpCode(InstrOpCode opCode)
{

    for (size_t i = 0; i < sizeof(instructions) / sizeof(InstructionMeta); i++)
    {

        if (opCode == instructions[i].OpCode)
            return &instructions[i];
    }

    return NULL;
}

static int findArgSetIdx(const InstructionMeta *im, ArgSet args)
{

    for (size_t j = 0; j < sizeof(im->ArgSets) / sizeof(ArgSet); j++)
    {
        if (im->ArgSets[j].First == args.First &&
            im->ArgSets[j].Second == args.Second)

            return j;
    }

    return -1;
}

InstrEncDecErr NewInstruction(InstructionName name, Instruction *instr)
{

    const InstructionMeta *im = FindInsMetaByName(name);
    if (im == NULL)
        return INSTR_UNKNOWN;

    instr->im = im;

    ArgSet argSet = {.First = instr->Arg1.Type, .Second = instr->Arg2.Type};

    int argSetIdx = findArgSetIdx(im, argSet);
    if (argSetIdx < 0)
        return INSTR_WRONG_OPERANDS;

    instr->ArgSetIdx = (uint8_t)argSetIdx;

    return INSTR_OK;
}
