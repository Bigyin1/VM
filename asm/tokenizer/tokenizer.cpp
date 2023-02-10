#include "tokenizer/tokenizer.hpp"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "errors.hpp"
#include "tockCheckers.hpp"

static Token* getNextFreeToken(Tokenizer* t)
{
    if (t->tokensSz < t->tokensCap)
    {
        t->tokensSz++;
        return t->tokens + t->tokensSz - 1;
    }

    size_t newCap  = t->tokensCap * 2;
    Token* newToks = (Token*)realloc(t->tokens, newCap * sizeof(Token));
    if (newToks == NULL)
        return NULL;

    t->currToken = newToks + t->tokensCap;
    memset(t->currToken, 0, sizeof(Token) * (newCap - t->tokensCap));

    t->tokens    = newToks;
    t->tokensCap = newCap;

    t->tokensSz++;
    return t->tokens + t->tokensSz - 1;
}

typedef size_t (*TockCheckFunc)(const char* input, Token* tok);

static TockCheckFunc tockCheckers[] = {
    checkSpaceToken,        checkSimpleTokens,   checkHexNum,
    checkNumberToken,       checkAsciiCharToken, checkLabelDefToken,
    checkInstrPostfixToken, checkRegisterToken,  checkIdToken,
};

static bool checkTokens(Tokenizer* t)
{

    for (size_t i = 0; i < sizeof(tockCheckers) / sizeof(TockCheckFunc); i++)
    {
        size_t tokLen = tockCheckers[i](t->input, t->currToken);
        if (tokLen == 0)
            continue;

        t->column += tokLen;
        t->input += tokLen;

        if (t->currToken->type == ASM_T_NL)
        {
            t->line++;
            t->column = 1;
        }

        return true;
    }

    return false;
}

TokErrCode Tokenize(Tokenizer* t)
{

    char* textStart = t->input;

    for (; *t->input != '\0';)
    {
        t->currToken = getNextFreeToken(t);
        if (t->currToken == NULL)
        {
            free(textStart);
            return TOK_SYTEM_ERROR;
        }

        t->currToken->column = t->column;
        t->currToken->line   = t->line;

        if (checkTokens(t))
            continue;

        if (addUnknownTokenError(t) < 0)
        {
            free(textStart);
            return TOK_SYTEM_ERROR;
        }
    }

    t->currToken = getNextFreeToken(t);
    if (t->currToken == NULL)
    {
        free(textStart);
        return TOK_SYTEM_ERROR;
    }

    t->currToken->type = ASM_T_EOF;

    t->currToken = NULL;

    free(textStart);

    return TOK_OK;
}

TokErrCode TokenizerInit(Tokenizer* t, char* input)
{
    t->input  = input;
    t->column = 1;
    t->line   = 1;

    t->currToken = NULL;
    t->tokens    = (Token*)calloc(128, sizeof(Token));
    if (t->tokens == NULL)
        return TOK_SYTEM_ERROR;

    t->tokensCap = 128;
    t->tokensSz  = 0;

    return TOK_OK;
}

Token* getNextToken(Tokenizer* t)
{
    assert(t != NULL);

    if (t->currToken == NULL)
    {
        t->currToken = t->tokens;
        return t->currToken;
    }

    t->currToken++;
    if (t->currToken->type == ASM_T_EOF)
        return NULL;

    return t->currToken;
}

Token* saveCurrToken(Tokenizer* t)
{
    assert(t != NULL);

    t->saved = t->currToken;

    return t->currToken;
}

Token* restoreSavedToken(Tokenizer* t)
{
    assert(t != NULL);

    t->currToken = t->saved;

    return t->currToken;
}

void tokenizerFree(Tokenizer* t)
{
    assert(t != NULL);

    while (t->userErrors != NULL)
    {
        TokenizerError* err = t->userErrors;

        free(err->text);
        t->userErrors = t->userErrors->next;
        free(err);
    }

    free(t->tokens);
}
