#include <assert.h>
#include <string.h>
#include "parser.hpp"
#include "utils.hpp"

static asm_ecode parseCommandArg(parser_s *parser, commandNode *node, Argument *arg) {
    assert(parser != NULL);
    assert(node != NULL);


    char *val = currTokenVal(parser);
    if (currTokenType(parser) == ASM_T_LABEL) {
        eatToken(parser, ASM_T_LABEL);

        arg->Type = ArgImm;

    } else if (currTokenType(parser) == ASM_T_FLOAT) {

        eatToken(parser, ASM_T_FLOAT);

        arg->Imm = currTokenDblNumVal(parser);
        arg->Type = ArgImm;

    } else if (currTokenType(parser) == ASM_T_INTEGER) {

        eatToken(parser, ASM_T_INTEGER);

        arg->Imm = currTokenIntNumVal(parser);
        arg->Type = ArgImm;

    } else if (currTokenType(parser) == ASM_T_ID) {
        eatToken(parser, ASM_T_ID);

        arg->RegNum = val;
        arg->type |= ARG_REG;
        arg->sz += regArgSize;

    } else {

        if (eatToken(parser, ASM_T_L_PAREN) != E_ASM_OK)
            return E_ASM_ERR;

        val = currTokenVal(parser);
        if (eatToken(parser, ASM_T_ID) != E_ASM_OK)
            return E_ASM_ERR;

        if (eatToken(parser, ASM_T_R_PAREN) != E_ASM_OK)
            return E_ASM_ERR;

        arg->reg = val;
        arg->type |= ARG_REG;
        arg->type |= ARG_ADDR;
        arg->sz += regArgSize;

    }

    return E_ASM_OK;
}


static asm_ecode parseCommandNode(parser_s *parser, commandNode *node) {
    assert(parser != NULL);
    assert(node != NULL);

    char *name = currTokenVal(parser);


    if (currTokenType(parser) == ASM_T_LABEL) {
        node->label = name;
        eatToken(parser, ASM_T_LABEL);
        eatBlanks(parser);
    }

    node->line = parser->toks->currToken->line;
    name = currTokenVal(parser);
    if (eatToken(parser, ASM_T_ID) != E_ASM_OK)
        return E_ASM_ERR;

    node->instrName = name;
    eatToken(parser, ASM_T_SPACE);

    if (parseCommandArg(parser, node, &node->instr.Arg1) != E_ASM_OK)
        return E_ASM_ERR;


    return E_ASM_OK;
}

static asm_ecode parseCodeNode(parser_s *parser, codeNode *node) {
    assert(parser != NULL);
    assert(node != NULL);

    commandNode *cmds = node->commands;
    getNextToken(parser->toks);

    eatBlanks(parser);


    while (1) {

        if (parseCommandNode(parser, cmds++) != E_ASM_OK)
            return E_ASM_ERR;

        if (currTokenType(parser) == ASM_T_EOF)
            break;

        eatSP(parser);

        if (eatToken(parser, ASM_T_NL) != E_ASM_OK)
            return E_ASM_ERR;

        eatBlanks(parser);

        if (currTokenType(parser) == ASM_T_EOF)
            break;
    }

    return E_ASM_OK;
}


asm_ecode parseTokens(parser_s *parser) {
    assert(parser != NULL);

    if (parseCodeNode(parser, &parser->prog) != E_ASM_OK)
        return E_ASM_ERR;

    return E_ASM_OK;
}
