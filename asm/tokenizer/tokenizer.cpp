#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenizer.hpp"

static const char *spaces = " \t";

const token_meta_s generalTokens[] = {
    {ASM_T_L_PAREN, "["},
    {ASM_T_R_PAREN, "]"},
    {ASM_T_L_SIMP_PAREN, "("},
    {ASM_T_R_SIMP_PAREN, ")"},
    {ASM_T_COMMENT, "#"},
    {ASM_T_COMMA, ","},
    {ASM_T_NL, "\n"},
};

static bool checkSimpleToken(tokenizer_s *t, e_asm_token_type type, const char *tokVal)
{

    size_t tokLen = strlen(tokVal);
    if (strncmp(t->input, tokVal, tokLen) != 0)
        return false;

    t->currToken->type = type;

    t->column += tokLen;
    t->input += tokLen;
    return true;
}

static bool checkLabelToken(tokenizer_s *t)
{

    int wordLen = 0;

    short colon = 0;

    if (sscanf(t->input, ":%"
                         "24"
                         "[a-zA-Z0-9_]%n",
               t->currToken->val, &wordLen) == 0 &&
        sscanf(t->input, "%"
                         "24"
                         "[a-zA-Z0-9_]%1[:]%n",
               t->currToken->val, &colon, &wordLen) != 2)
        return false;

    t->currToken->type = ASM_T_LABEL;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkSectNameToken(tokenizer_s *t)
{

    int wordLen = 0;

    short colon = 0;

    if (sscanf(t->input, ".%"
                         "24"
                         "[a-zA-Z]%n",
               t->currToken->val, &wordLen) == 0)
        return false;

    t->currToken->type = ASM_T_SECTION_NAME;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkNumberToken(tokenizer_s *t)
{

    int numLen = 0;
    sscanf(t->input, "%*[0-9.+-]%n", &numLen);
    if (numLen == 0)
        return false;

    char sign = t->input[0];

    uint64_t num = 0;
    int read = 0;
    if (sign == '-')
    {
        if (sscanf(t->input, "%lld%n", &num, &read) == 0)
            return false;
    }
    else
    {
        if (sscanf(t->input, "%llu%n", &num, &read) == 0)
            return false;
    }

    if (read < numLen)
    {
        double dblNum = 0;
        sscanf(t->input, "%lf%n", &dblNum, &read);
        if (read < numLen)
            return false;

        memcpy(&t->currToken->numVal, &dblNum, sizeof(double));
        t->currToken->type = ASM_T_FLOAT;
    }
    else if (sign == '-')
    {
        t->currToken->numVal = num;
        t->currToken->type = ASM_T_SIGNED_INT;
    }
    else
    {
        t->currToken->numVal = num;
        t->currToken->type = ASM_T_UNSIGNED_INT;
    }

    t->column += numLen;
    t->input += numLen;
    return true;
}

static bool checkIdToken(tokenizer_s *t)
{

    int wordLen = 0;
    sscanf(t->input, "%*[0-9a-zA-Z]%n", &wordLen);
    if (wordLen == 0)
        return false;

    sscanf(t->input, "%24[0-9a-zA-Z]", t->currToken->val);
    t->currToken->type = ASM_T_ID;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkAsciiCharToken(tokenizer_s *t)
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

    t->currToken->type = ASM_T_UNSIGNED_INT;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}

static bool checkSimpleTokens(tokenizer_s *t)
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

asm_ecode tokenize(tokenizer_s *t)
{

    asm_ecode err = E_ASM_OK;
    char *textStart = t->input;

    t->currToken = t->tokens;

    for (; *t->input != '\0'; t->currToken++)
    {
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

        if (checkSectNameToken(t))
            continue;

        if (checkAsciiCharToken(t))
            continue;

        if (checkLabelToken(t))
            continue;

        if (checkNumberToken(t))
            continue;

        if (checkSimpleTokens(t))
            continue;

        if (checkIdToken(t))
            continue;

        printf("asm: unknown token at line: %zu, column: %zu\n",
               t->currToken->line,
               t->currToken->column);
        err = E_ASM_INSUFF_TOKEN;
        break;
    }

    t->currToken->type = ASM_T_EOF;
    t->currToken = NULL;

    free(textStart);

    return err;
}

asm_ecode tokenizerInit(tokenizer_s *t, char *input)
{
    t->input = input;
    t->column = 1;
    t->line = 1;

    t->currToken = NULL;
    t->tokens = (token_s *)calloc(1024, sizeof(token_s)); // TODO: change to list
    if (t->tokens == NULL)
    {
        perror("asm: ");
        return E_ASM_ERR;
    }

    return E_ASM_OK;
}

token_s *getNextToken(tokenizer_s *t)
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

token_s *saveCurrToken(tokenizer_s *t)
{
    assert(t != NULL);

    t->saved = t->currToken;

    return t->currToken;
}

token_s *restoreSavedToken(tokenizer_s *t)
{
    assert(t != NULL);

    t->currToken = t->saved;

    return t->currToken;
}

token_s *peekNextToken(tokenizer_s *t)
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

void tokenizerFree(tokenizer_s *t)
{
    assert(t != NULL);

    free(t->tokens);
}
