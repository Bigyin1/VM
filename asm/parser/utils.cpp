
#include <assert.h>
#include "utils.hpp"
#include "tokenizer/tokenizer.hpp"
#include "errors.hpp"

ParserErrCode eatToken(Parser *p, TokenType type)
{
    assert(p != NULL);

    if (currTokenType(p) != type)
    {
        ParserError *err = addNewParserError(p, PARSER_INSUFF_TOKEN);
        err->got = currTokenType(p);
        err->expected = type;
        err->line = currTokenLine(p);
        err->column = currTokenColumn(p);

        return PARSER_INSUFF_TOKEN;
    }

    getNextToken(p->toks);
    return PARSER_OK;
}

void eatNL(Parser *p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_NL)
    {
        getNextToken(p->toks);
    }
}

void eatSP(Parser *p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_SPACE ||
           currTokenType(p) == ASM_T_COMMENT)
    {
        getNextToken(p->toks);
    }
}

void eatBlanks(Parser *p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_NL ||
           currTokenType(p) == ASM_T_SPACE ||
           currTokenType(p) == ASM_T_COMMENT)
    {
        getNextToken(p->toks);
    }
}

void eatUntillNL(Parser *p)
{

    assert(p != NULL);
    while (currTokenType(p) != ASM_T_NL)
        getNextToken(p->toks);
}
