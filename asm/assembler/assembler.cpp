#include <assert.h>
#include <string.h>
#include "../errors.hpp"
#include "../../vm/cpu/instructions.hpp"
#include "assembler.hpp"



static bool instrEncode(const commandNode *cmd, FILE *out) {
    assert(cmd != NULL);
    assert(out != NULL);

    instrBin_s instr = {
        .arg1Type = cmd->args[0].type,
        .arg1Value = cmd->args[0].encVal,
        .arg2Type = cmd->args[1].type,
        .arg2Value = cmd->args[1].encVal,
    };

    const instrMeta *im = findInstrByName(cmd->instrName);
    if (im == NULL)
        return false;

    instr.opCode = im->opCode;

    if ((instr.arg1Type & im->allowedArgs[0]) == 0)
        return false;

    if ((instr.arg2Type & im->allowedArgs[1]) == 0)
        return false;

    fwrite(&instr, sizeof(instr), 1, out); //check error;

    return true;
}




static void evalOffsets(codeNode *code) {
    assert(code != NULL);

    size_t currOffset = code->addr;

    for (size_t i = 0; code->commands[i].instrName != NULL; i++) {

       code->commands[i].offset = currOffset;

       currOffset += sizeof(instrBin_s);
    }
}


static e_asm_codes getLabelOffset(commandNode *cmds, const char *label, size_t *res) {
    assert(label != NULL);

    for (size_t i = 0; cmds[i].instrName != NULL; i++) {

        if (cmds[i].label == NULL)
            continue;

        if (strcmp(label, cmds[i].label) == 0) {
            *res = cmds[i].offset;
            return E_ASM_OK;
        }
    }

    return E_ASM_ERR;
}

static bool expandCommonArg(instrArgument *arg) {
    assert(arg != NULL);

    if (arg->val == NULL) {
        arg->type = ARG_EMPTY;
        return true;
    }

    if (findRegByName(arg->val) != NULL) {

        arg->encVal = findRegByName(arg->val)->regCode;
        arg->type = arg->isAddr ? ARG_REG_ADDR : ARG_REG;
        return true;
    }
    if (sscanf(arg->val, "%hd", &arg->encVal) == 1) {

        arg->type = arg->isAddr ? ARG_IMM_ADDR : ARG_IMM;
        return true;
    }

    return false;
}


static e_asm_codes substLabelsArgs(commandNode *commands) {
    assert(commands != NULL);

    for (size_t i = 0; commands[i].instrName != NULL; i++) {

        for (size_t argIdx = 0; argIdx < instrArgsMaxCount; argIdx++) {

            instrArgument *arg = &commands[i].args[argIdx];
            size_t offset = 0;

            if (!expandCommonArg(arg)) {
                if (getLabelOffset(commands, arg->val, &offset) == E_ASM_ERR) {
                    printf("asm: line: %zu : label \"%s\" does not exist\n", commands[i].line, arg->val);
                    return E_ASM_ERR;
                }
                arg->encVal = (short)offset;
                arg->type = arg->isAddr ? ARG_IMM_ADDR : ARG_IMM;
            }
        }
    }
    return E_ASM_OK;
}


static e_asm_codes writeCode(commandNode *commands, FILE *out) {
    assert(commands != NULL);

    size_t i = 0;
    for (; commands[i].instrName != NULL; i++) {

        if (!instrEncode(&commands[i], out)) {
            printf("asm: line: %zu : instruction invalid\n", commands[i].line);
            return E_ASM_ERR;
        }
    }

    commands[i].instrName = "end";
    commands[i].args[0].type = ARG_EMPTY;
    commands[i].args[1].type = ARG_EMPTY;
    instrEncode(&commands[i], out);

    return E_ASM_OK;
}


asm_ecode assemble(assembler_s *as) {
    assert(as != NULL);

    evalOffsets(&as->prog->code);
    if (substLabelsArgs(as->prog->code.commands) != E_ASM_OK)
        return E_ASM_ERR;

    if (writeCode(as->prog->code.commands, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    return E_ASM_OK;
}
