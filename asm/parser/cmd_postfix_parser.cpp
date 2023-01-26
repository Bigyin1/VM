#include <string.h>
#include "utils.hpp"
#include "errors.hpp"
#include "cmd_postfix_parser.hpp"

static ParserErrCode parseJumpPostfix(commandNode *node, const char *postfix)
{
    if (strcmp(postfix, "eq") == 0)
    {
        node->instr.JmpType = JumpEQ;
        return PARSER_OK;
    }

    if (strcmp(postfix, "neq") == 0)
    {
        node->instr.JmpType = JumpNEQ;
        return PARSER_OK;
    }

    if (strcmp(postfix, "g") == 0)
    {
        node->instr.JmpType = JumpG;
        return PARSER_OK;
    }

    if (strcmp(postfix, "ge") == 0)
    {
        node->instr.JmpType = JumpGE;
        return PARSER_OK;
    }

    if (strcmp(postfix, "l") == 0)
    {
        node->instr.JmpType = JumpL;
        return PARSER_OK;
    }

    if (strcmp(postfix, "le") == 0)
    {
        node->instr.JmpType = JumpLE;
        return PARSER_OK;
    }

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

    if (strcmp(postfix, "b") == 0)
    {
        node->instr.DataSz = DataByte;
        return PARSER_OK;
    }

    if (strcmp(postfix, "s") == 0)
    {
        node->instr.DataSz = DataWord;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    if (strcmp(postfix, "bs") == 0)
    {
        node->instr.DataSz = DataByte;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    if (strcmp(postfix, "db") == 0)
    {
        node->instr.DataSz = DataDByte;
        return PARSER_OK;
    }

    if (strcmp(postfix, "dbs") == 0)
    {
        node->instr.DataSz = DataDByte;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    if (strcmp(postfix, "hw") == 0)
    {
        node->instr.DataSz = DataHalfWord;
        return PARSER_OK;
    }

    if (strcmp(postfix, "hws") == 0)
    {
        node->instr.DataSz = DataHalfWord;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    ParserError *err = addNewParserError(parser, PARSER_BAD_CMD_POSTFIX);

    err->token = postfix;
    err->line = line;
    err->column = column;

    return PARSER_OK;
}
