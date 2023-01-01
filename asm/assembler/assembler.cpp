#include <assert.h>
#include <string.h>
#include <stdint.h>
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

    uint32_t ie = im->encode(&instr);

    fwrite(&ie, sizeof(uint32_t), 1, out); //check error;

    return true;
}




static void evalOffsets(codeNode *code) {
    assert(code != NULL);

    size_t currOffset = code->addr;

    for (size_t i = 0; code->commands[i].instrName != NULL; i++) {
        currOffset += 1;
        instrArgument *arg = &code->commands[i].arg;

        if (arg->isAddr || arg.)

        currOffset += sizeof(uint32_t);
    }
}


static e_asm_codes getLabelOffset(commandNode *cmds, const char *label, size_t *res) {
    assert(label != NULL);
    assert(res != NULL);

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
    if (sscanf(arg->val, "%lf", &arg->encVal) == 1) {

        arg->type = arg->isAddr ? ARG_IMM_ADDR : ARG_IMM;
        return true;
    }

    return false;
}


static e_asm_codes substLabelsArgs(commandNode *commands) {
    assert(commands != NULL);

    for (size_t i = 0; commands[i].instrName != NULL; i++) {

        instrArgument *arg = &commands[i].arg;
        size_t offset = 0;

        if (!expandCommonArg(arg)) {
            if (getLabelOffset(commands, arg->val, &offset) == E_ASM_ERR) {
                printf("asm: line: %zu : label \"%s\" does not exist\n", commands[i].line, arg->val);
                return E_ASM_ERR;
            }
            arg->encVal = offset;
            arg->type = arg->isAddr ? ARG_IMM_ADDR : ARG_IMM;
        }
    }
    return E_ASM_OK;
}


static e_asm_codes writeCode(commandNode *commands, FILE *out) {
    assert(commands != NULL);

    for (size_t i = 0; commands[i].instrName != NULL; i++) {

        if (!instrEncode(&commands[i], out)) {
            printf("asm: line: %zu : instruction invalid\n", commands[i].line);
            return E_ASM_ERR;
        }
    }

    return E_ASM_OK;
}


asm_ecode assemble(assembler_s *as) {
    assert(as != NULL);

    evalOffsets(as->prog);
    if (substLabelsArgs(as->prog->commands) != E_ASM_OK)
        return E_ASM_ERR;

    if (writeCode(as->prog->commands, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    return E_ASM_OK;
}
