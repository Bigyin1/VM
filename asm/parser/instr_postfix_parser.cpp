#include "instr_postfix_parser.hpp"

#include <string.h>

#include "errors.hpp"
#include "utils.hpp"

static ParserErrCode parseJumpPostfix(CommandNode* node, const char* postfix)
{
#define JMP_POSTFIX(pfix, jmpType)                                                                 \
    if (strcmp(#pfix, postfix) == 0)                                                               \
    {                                                                                              \
        node->instr.JmpType = jmpType;                                                             \
        return PARSER_OK;                                                                          \
    }

#include "tmpl/jmpTypePostfix.inc"

#undef JMP_POSTFIX

    return PARSER_INSUFF_TOKEN;
}

ParserErrCode parseInstrPostfix(Parser* parser, CommandNode* node)
{
    if (currTokenType(parser) != ASM_T_INSTR_POSTFIX)
        return PARSER_OK;

    const char* postfix = currTokenVal(parser);

    eatToken(parser, ASM_T_INSTR_POSTFIX);

    if (parseJumpPostfix(node, postfix) == PARSER_OK)
        return PARSER_OK;

#define DATA_POSTFIX(pfix, dataSz, signExt)                                                        \
    if (strcmp(#pfix, postfix) == 0)                                                               \
    {                                                                                              \
        node->instr.DataSz  = dataSz;                                                              \
        node->instr.SignExt = signExt;                                                             \
        return PARSER_OK;                                                                          \
    }

#include "tmpl/cmdDataPostfixes.inc"

#undef DATA_POSTFIX

    addBadInstrPostfixError(parser, postfix);

    return PARSER_BAD_CMD_POSTFIX;
}
