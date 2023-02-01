#include <string.h>
#include "utils.hpp"
#include "errors.hpp"
#include "cmd_postfix_parser.hpp"

static ParserErrCode parseJumpPostfix(commandNode *node, const char *postfix)
{

#define JMP_POSTFIX(pfix, jmpType)     \
    if (strcmp(#pfix, postfix) == 0)   \
    {                                  \
        node->instr.JmpType = jmpType; \
        return PARSER_OK;              \
    }

#include "tmpl/jmpTypePostfix.inc"

#undef JMP_POSTFIX

    return PARSER_INSUFF_TOKEN;
}

ParserErrCode parseInstrPostfix(Parser *parser, commandNode *node)
{

    const char *postfix = currTokenVal(parser);
    size_t line = currTokenLine(parser);
    size_t column = currTokenColumn(parser);

    if (eatToken(parser, ASM_T_ID) != PARSER_OK)
        return PARSER_BAD_COMMAND;

    if (parseJumpPostfix(node, postfix) == PARSER_OK)
        return PARSER_OK;

#define DATA_POSTFIX(pfix, dataSz, signExt) \
    if (strcmp(#pfix, postfix) == 0)        \
    {                                       \
        node->instr.DataSz = dataSz;        \
        node->instr.SignExt = signExt;      \
        return PARSER_OK;                   \
    }

#include "tmpl/cmdDataPostfixes.inc"

#undef DATA_POSTFIX

    ParserError *err = addNewParserError(parser, PARSER_BAD_CMD_POSTFIX);

    err->token = postfix;
    err->line = line;
    err->column = column;

    return PARSER_BAD_CMD_POSTFIX;
}
