#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenizer/tokenizer.hpp"
#include "errors.hpp"

static const char *spaces = " \t";

typedef struct token_meta_s
{

    TokenType type;
    const char *val;

} token_meta_s;

const token_meta_s generalTokens[] = {
    {ASM_T_L_PAREN, "["},
    {ASM_T_R_PAREN, "]"},
    {ASM_T_COMMENT, "#"},
    {ASM_T_COMMA, ","},
    {ASM_T_NL, "\n"},
};

static bool checkSimpleToken(Tokenizer *t, TokenType type, const char *tokVal)
{

    size_t tokLen = strlen(tokVal);
    if (strncmp(t->input, tokVal, tokLen) != 0)
        return false;

    t->currToken->type = type;

    t->column += tokLen;
    t->input += tokLen;
    return true;
}

static bool checkLabelToken(Tokenizer *t)
{

    int wordLen = 0;

    short colon = 0;

    if (sscanf(t->input, ":%" XSTR(MAX_TOKEN_LEN) "[.a-zA-Z0-9_]%n",
               t->currToken->val, &wordLen) == 0 &&
        sscanf(t->input, "%" XSTR(MAX_TOKEN_LEN) "[.a-zA-Z0-9_]%1[:]%n",
               t->currToken->val, &colon, &wordLen) != 2)
        return false;

    t->currToken->type = ASM_T_LABEL;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkSectNameToken(Tokenizer *t)
{

    int wordLen = 0;

    if (sscanf(t->input, ".%" XSTR(MAX_TOKEN_LEN) "[a-zA-Z]%n",
               t->currToken->val, &wordLen) == 0)
        return false;

    t->currToken->type = ASM_T_SECTION_NAME;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkHexNum(Tokenizer *t)
{

    int wordLen = 0;

    if (sscanf(t->input, "0x"
                         "%llx%n",
               &t->currToken->numVal, &wordLen) == 0)
        return false;

    t->currToken->type = ASM_T_INT;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkNumberToken(Tokenizer *t)
{

    if (checkHexNum(t))
        return true;

    int numLen = 0;
    sscanf(t->input, "%*[0-9.+-]%n", &numLen);
    if (numLen == 0)
        return false;

    int read = 0;

    if (sscanf(t->input, "%lld%n", &t->currToken->numVal, &read) == 0)
        return false;

    if (read < numLen)
    {
        double dblNum = 0;
        sscanf(t->input, "%lf%n", &dblNum, &read);
        if (read < numLen)
            return false;

        memcpy(&t->currToken->numVal, &dblNum, sizeof(double));
        t->currToken->type = ASM_T_FLOAT;
    }
    else
        t->currToken->type = ASM_T_INT;

    t->column += numLen;
    t->input += numLen;
    return true;
}

static bool checkIdToken(Tokenizer *t)
{

    int wordLen = 0;
    sscanf(t->input, "%*[0-9a-zA-Z]%n", &wordLen);
    if (wordLen == 0)
        return false;

    sscanf(t->input, "%" XSTR(MAX_TOKEN_LEN) "[0-9a-zA-Z]", t->currToken->val);
    t->currToken->type = ASM_T_ID;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkAsciiCharToken(Tokenizer *t)
{

    int wordLen = 0;
    char c = 0;
    char backslash[2] = {0};
    char qoute[2] = {0};
    if (sscanf(t->input, "'%1[\\]%1c%1[']%n", backslash, &c, qoute, &wordLen) != 3 &&
        sscanf(t->input, "'%1c%1[']%n", &c, qoute, &wordLen) != 2)
        return false;

    if (backslash[0] && c != '\\')
        c &= 0b00111111;

    t->currToken->numVal = c;

    t->currToken->type = ASM_T_INT;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkInstrPostfixToken(Tokenizer *t)
{
    int wordLen = 0;
    char rparen[2] = {0};
    if (sscanf(t->input, "(%3[a-z]%1[)]%n", t->currToken->val, rparen, &wordLen) != 2)
        return false;

    t->currToken->type = ASM_T_INSTR_POSTFIX;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkSimpleTokens(Tokenizer *t)
{
    for (size_t i = 0; i < sizeof(generalTokens) / sizeof(generalTokens[0]); i++)
    {
        if (checkSimpleToken(t, generalTokens[i].type, generalTokens[i].val))
        {
            if (generalTokens[i].type == ASM_T_NL)
            {
                t->line++;
                t->column = 1;
                return true;
            }
            if (generalTokens[i].type == ASM_T_COMMENT)
            {
                int len = 0;
                sscanf(t->input, "%*[^\n]%n", &len);
                t->column += len;
                t->input += len;
            }
            return true;
        }
    }
    return false;
}

static Token *reallocTokens(Tokenizer *t)
{
    if (t->tokensSz < t->tokensCap)
    {
        t->tokensSz++;
        return t->tokens + t->tokensSz - 1;
    }

    size_t newCap = t->tokensCap * 2;
    Token *newToks = (Token *)realloc(t->tokens, newCap * sizeof(Token));
    if (newToks == NULL)
        return NULL;

    t->currToken = newToks + t->tokensCap;
    memset(t->currToken, 0, sizeof(Token) * (newCap - t->tokensCap));
    t->tokens = newToks;
    t->tokensCap = newCap;

    t->tokensSz++;
    return t->tokens + t->tokensSz - 1;
}

TokErrCode Tokenize(Tokenizer *t)
{

    char *textStart = t->input;

    for (; *t->input != '\0';)
    {
        t->currToken = reallocTokens(t);
        if (t->currToken == NULL)
        {
            free(textStart);
            return TOK_SYTEM_ERROR;
        }

        t->currToken->column = t->column;
        t->currToken->line = t->line;

        size_t tokLen = strspn(t->input, spaces);
        if (tokLen != 0)
        {
            t->currToken->type = ASM_T_SPACE;
            t->input += tokLen;
            t->column += tokLen;
            continue;
        }

        if (t->currToken->type != ASM_T_EOF)
            continue;

        if (checkSimpleTokens(t))
            continue;

        if (checkNumberToken(t))
            continue;

        if (checkSectNameToken(t))
            continue;

        if (checkAsciiCharToken(t))
            continue;

        if (checkLabelToken(t))
            continue;

        if (checkInstrPostfixToken(t))
            continue;

        if (checkIdToken(t))
            continue;

        if (addUnknownTokenError(t) < 0)
        {
            free(textStart);
            return TOK_SYTEM_ERROR;
        }
    }

    t->currToken = reallocTokens(t);
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

TokErrCode TokenizerInit(Tokenizer *t, char *input)
{
    t->input = input;
    t->column = 1;
    t->line = 1;

    t->currToken = NULL;
    t->tokens = (Token *)calloc(128, sizeof(Token));
    if (t->tokens == NULL)
        return TOK_SYTEM_ERROR;

    t->tokensCap = 128;
    t->tokensSz = 0;

    return TOK_OK;
}

Token *getNextToken(Tokenizer *t)
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

Token *saveCurrToken(Tokenizer *t)
{
    assert(t != NULL);

    t->saved = t->currToken;

    return t->currToken;
}

Token *restoreSavedToken(Tokenizer *t)
{
    assert(t != NULL);

    t->currToken = t->saved;

    return t->currToken;
}

Token *peekNextToken(Tokenizer *t)
{
    assert(t != NULL);

    if (t->currToken == NULL)
    {
        t->currToken = t->tokens;
        return t->currToken;
    }

    if (t->currToken->type == ASM_T_EOF)
        return t->currToken;

    return t->currToken + 1;
}

void tokenizerFree(Tokenizer *t)
{
    assert(t != NULL);

    while (t->userErrors != NULL)
    {
        TokenizerError *err = t->userErrors;

        free(err->text);
        t->userErrors = t->userErrors->next;
        free(err);
    }

    free(t->tokens);
}
