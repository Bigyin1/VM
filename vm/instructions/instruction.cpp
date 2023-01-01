#include <assert.h>
#include <ctype.h>
#include "instruction.hpp"



InstrErr NewInstruction(InstructionName name, Instruction instr, size_t *sz) {

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
