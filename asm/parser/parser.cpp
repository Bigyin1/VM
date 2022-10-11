#include <assert.h>
#include <string.h>
#include "parser.hpp"
#include "utils.hpp"

static const char *codeSectName = "code";
static const char *dataSectName = "data";

static asm_ecode parseCommandArgs(parser_s *parser, commandNode *node) {
    assert(parser != NULL);
    assert(node != NULL);

    for (size_t i = 0; i < instrArgsMaxCount; i++) {
        instrArgument *arg = node->args + i;

        char *val = currTokenVal(parser);
        if (currTokenType(parser) == ASM_T_WORD) {

            eatToken(parser, ASM_T_WORD);
            arg->val = val;
            arg->isAddr = false;

        } else {

            if (eatToken(parser, ASM_T_L_PAREN) != E_ASM_OK)
                return E_ASM_ERR;

            val = currTokenVal(parser);
            if (eatToken(parser, ASM_T_WORD) != E_ASM_OK)
                return E_ASM_ERR;

            if (eatToken(parser, ASM_T_R_PAREN) != E_ASM_OK)
                return E_ASM_ERR;

            arg->val = val;
            arg->isAddr = true;
        }

        eatSP(parser);

        if (currTokenType(parser) == ASM_T_COMMA)
            eatToken(parser, ASM_T_COMMA);
        else
            return E_ASM_OK;

        eatSP(parser);
    }

    return E_ASM_OK;
}


static asm_ecode parseCommandNode(parser_s *parser, commandNode *node) {
    assert(parser != NULL);
    assert(node != NULL);

    char *name = currTokenVal(parser);

    node->line = parser->toks->currToken->line;

    if (eatToken(parser, ASM_T_WORD) != E_ASM_OK)
        return E_ASM_ERR;


    if (currTokenType(parser) == ASM_T_COLON) {
        eatToken(parser, ASM_T_COLON);
        node->label = name;

        eatBlanks(parser);

        name = currTokenVal(parser);
        if (eatToken(parser, ASM_T_WORD) != E_ASM_OK)
            return E_ASM_ERR;
    }

    node->instrName = name;

    if (peekNextToken(parser->toks)->type != ASM_T_NL)
        if (eatToken(parser, ASM_T_SPACE) != E_ASM_OK)
            return E_ASM_ERR;

    if (parseCommandArgs(parser, node) != E_ASM_OK)
        return E_ASM_ERR;


    return E_ASM_OK;
}

static asm_ecode parseCodeNode(parser_s *parser, codeNode *node) {
    assert(parser != NULL);
    assert(node != NULL);

    commandNode *cmds = node->commands;

    eatSP(parser);

    if (eatToken(parser, ASM_T_NL) != E_ASM_OK)
        return E_ASM_ERR;

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


static asm_ecode parseProgramNode(parser_s *parser, programNode *node) {
    assert(parser != NULL);
    assert(node != NULL);

    getNextToken(parser->toks);
    eatNL(parser);

    if (eatToken(parser, ASM_T_DOT) != E_ASM_OK)
        return E_ASM_ERR;

    char *sectionName = currTokenVal(parser);

    if (eatToken(parser, ASM_T_WORD) != E_ASM_OK)
        return E_ASM_ERR;

    if (strcmp(sectionName, codeSectName) != 0) {
        printf("asm: usupported file section: %s\n", sectionName);
        return E_ASM_ERR;
    }

    if (parseCodeNode(parser, &node->code) != E_ASM_OK)
        return E_ASM_ERR;


    return E_ASM_OK;
}



asm_ecode parseTokens(parser_s *parser) {
    assert(parser != NULL);

    return parseProgramNode(parser, &parser->prog);
}
