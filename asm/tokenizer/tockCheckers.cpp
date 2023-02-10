#include "tockCheckers.hpp"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

size_t checkLabelDefToken(const char* input, Token* tok)
{

    unsigned tokLen = 0;

    short colon = 0;

    if (sscanf(input, "%" XSTR(MAX_TOKEN_LEN) "[.a-zA-Z0-9_]%1[:]%n", tok->val, (char*)&colon,
               (int*)&tokLen) != 2)
        return 0;

    tok->type = ASM_T_LABEL_DEF;

    return tokLen;
}

size_t checkHexNum(const char* input, Token* tok)
{

    unsigned tokLen = 0;

    if (sscanf(input,
               "0x"
               "%llx%n",
               (uint64_t*)&tok->numVal, (int*)&tokLen) == 0)
        return 0;

    tok->type = ASM_T_INT;

    return tokLen;
}

size_t checkNumberToken(const char* input, Token* tok)
{

    unsigned numLen = 0;

    sscanf(input, "%*[0-9.+-]%n", (int*)&numLen);
    if (numLen == 0)
        return 0;

    unsigned read = 0;

    if (sscanf(input, "%lld%n", &tok->numVal, (int*)&read) == 0)
        return 0;

    if (read < numLen)
    {
        double dblNum = 0;
        sscanf(input, "%lf%n", &dblNum, (int*)&read);
        if (read < numLen)
            return 0;

        memcpy(&tok->numVal, &dblNum, sizeof(double));
        tok->type = ASM_T_FLOAT;
    }
    else
        tok->type = ASM_T_INT;

    return numLen;
}

size_t checkIdToken(const char* input, Token* tok)
{

    unsigned tokLen = 0;

    sscanf(input, "%" XSTR(MAX_TOKEN_LEN) "[._0-9a-zA-Z]%n", tok->val, (int*)&tokLen);
    if (tokLen == 0)
        return false;

    tok->type = ASM_T_ID;

    return tokLen;
}

size_t checkRegisterToken(const char* input, Token* tok)
{

    unsigned tokLen = 0;

    if (sscanf(input, "%%%" XSTR(MAX_TOKEN_LEN) "[0-9a-z]%n", tok->val, (int*)&tokLen) != 1)
        return 0;

    tok->type = ASM_T_REGISTER;

    return tokLen;
}

size_t checkAsciiCharToken(const char* input, Token* tok)
{

    unsigned tokLen = 0;
    char     c      = 0;

    short backslash = 0;
    short qoute     = 0;

    if (sscanf(input, "'%1[\\]%1c%1[']%n", (char*)&backslash, &c, (char*)&qoute, (int*)&tokLen) !=
            3 &&
        sscanf(input, "'%1c%1[']%n", &c, (char*)&qoute, (int*)&tokLen) != 2)
        return 0;

    if (backslash && c != '\\')
        c &= 0b00111111;

    tok->numVal = c;

    tok->type = ASM_T_INT;

    return tokLen;
}

size_t checkInstrPostfixToken(const char* input, Token* tok)
{
    unsigned tokLen = 0;

    short rparen = 0;

    if (sscanf(input, "(%3[a-z]%1[)]%n", tok->val, (char*)&rparen, (int*)&tokLen) != 2)
        return 0;

    tok->type = ASM_T_INSTR_POSTFIX;

    return tokLen;
}

size_t checkSpaceToken(const char* input, Token* tok)
{

    unsigned tokLen = 0;

    sscanf(input, "%*[\t ]%n", (int*)&tokLen);
    if (tokLen == 0)
        return tokLen;

    tok->type = ASM_T_SPACE;

    return tokLen;
}

typedef struct token_meta_s
{

    TokenType   type;
    const char* val;

} token_meta_s;

static const token_meta_s generalTokens[] = {
    {ASM_T_L_PAREN, "["}, {ASM_T_R_PAREN, "]"},       {ASM_T_COMMENT, "#"},
    {ASM_T_COMMA, ","},   {ASM_T_SECTION, "section"}, {ASM_T_NL, "\n"},
};

static size_t checkSimpleToken(const char* input, const char* tokVal)
{

    size_t tokLen = strlen(tokVal);
    if (strncmp(input, tokVal, tokLen) != 0)
        return 0;

    return tokLen;
}

size_t checkSimpleTokens(const char* input, Token* tok)
{
    for (size_t i = 0; i < sizeof(generalTokens) / sizeof(generalTokens[0]); i++)
    {
        size_t tokLen = checkSimpleToken(input, generalTokens[i].val);
        if (tokLen == 0)
            continue;

        if (generalTokens[i].type == ASM_T_COMMENT)
        {
            unsigned commentLen = 0;
            sscanf(input + tokLen, "%*[^\n]%n", (int*)&commentLen);
            tokLen += commentLen;
        }

        tok->type = generalTokens[i].type;

        return tokLen;
    }
    return 0;
}
