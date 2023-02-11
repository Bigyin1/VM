
#include "utils.hpp"

#include <assert.h>

#include "errors.hpp"
#include "tokenizer/tokenizer.hpp"

ParserErrCode eatToken(Parser* p, TokenType type)
{
    assert(p != NULL);

    if (currTokenType(p) != type)
    {
        ParserError* err = newParserError(p);

        err->code     = PARSER_INSUFF_TOKEN;
        err->got      = currTokenType(p);
        err->expected = type;
        err->line     = currTokenLine(p);
        err->column   = currTokenColumn(p);

        return PARSER_INSUFF_TOKEN;
    }

    getNextToken(p->toks);
    return PARSER_OK;
}

void eatNL(Parser* p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_NL)
    {
        getNextToken(p->toks);
    }
}

void eatSP(Parser* p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_SPACE || currTokenType(p) == ASM_T_COMMENT)
    {
        getNextToken(p->toks);
    }
}

void eatBlanks(Parser* p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_NL || currTokenType(p) == ASM_T_SPACE ||
           currTokenType(p) == ASM_T_COMMENT)
    {
        getNextToken(p->toks);
    }
}

void eatUntillNL(Parser* p)
{

    assert(p != NULL);
    while (currTokenType(p) != ASM_T_NL)
    {
        getNextToken(p->toks);
        if (currTokenType(p) == ASM_T_EOF)
            return;
    }
}

DataSize evalImmMinDataSz(int64_t val)
{

    if (val <= INT8_MAX && val >= INT8_MIN)
        return DataByte;
    if (val <= INT16_MAX && val >= INT16_MIN)
        return DataDByte;
    if (val <= INT32_MAX && val >= INT32_MIN)
        return DataHalfWord;
    return DataWord;
}
