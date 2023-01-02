
#include <assert.h>
#include "utils.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "../errors.hpp"

asm_ecode eatToken(parser_s *p, e_asm_token_type type)
{
    assert(p != NULL);

    if (currTokenType(p) != type)
    {
        printf(PARSE_ERROR, p->toks->currToken->line, p->toks->currToken->column);
        return E_ASM_ERR;
    }

    getNextToken(p->toks);
    return E_ASM_OK;
}

void eatNL(parser_s *p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_NL)
    {
        getNextToken(p->toks);
    }
}

void eatSP(parser_s *p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_SPACE ||
           currTokenType(p) == ASM_T_COMMENT)
    {
        getNextToken(p->toks);
    }
}

void eatBlanks(parser_s *p)
{
    assert(p != NULL);

    while (currTokenType(p) == ASM_T_NL ||
           currTokenType(p) == ASM_T_SPACE ||
           currTokenType(p) == ASM_T_COMMENT)
    {
        getNextToken(p->toks);
    }
}
