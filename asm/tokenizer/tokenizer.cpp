#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenizer.hpp"

static const char *spaces = " \t";


const token_meta_s generalTokens[] = {
  {ASM_T_L_PAREN, "["},
//   {ASM_T_PLUS, "+"},
//   {ASM_T_MINUS, "-"},
  {ASM_T_R_PAREN, "]"},
  {ASM_T_COMMENT, "#"},
  {ASM_T_COMMA, ","},
  {ASM_T_NL, "\n"},
};

static bool checkSimpleToken(tokenizer_s *t, e_asm_token_type type, const char *tokVal) {
    assert(t != NULL);
    assert(tokVal != NULL);

    size_t tokLen = strlen (tokVal);
    if (strncmp (t->input, tokVal, tokLen) != 0)
        return false;

    t->currToken->type = type;

    t->column += tokLen;
    t->input += tokLen;
    return true;
}


static bool checkLabelToken(tokenizer_s *t) {
    assert(t != NULL);

    int wordLen = 0;

    short colon = 0;
    if (sscanf(t->input, ":%24[a-zA-Z1-9]%n", t->currToken->val, &wordLen) == 0
        && sscanf(t->input, "%24[a-zA-Z1-9]%1[:]%n", t->currToken->val, &colon, &wordLen) != 2) // separates only spaces
        return false;

    t->currToken->type = ASM_T_LABEL;

    t->column += wordLen;
    t->input += wordLen;
    return true;
}


static bool checkNumberToken(tokenizer_s *t) {
    assert(t != NULL);

    int numLen = 0;
    sscanf(t->input, "%*[1-9.+-]%n", &numLen);

    int64_t num = 0;
    int read = 0;
    if (sscanf(t->input, "%ld%n", &num, &read) == 0)
        return false;

    if (read < numLen) {
        double dblNum = 0;
        sscanf(t->input, "%lf%n", &dblNum, &read);
        if (read < numLen)
            return false;

        t->currToken->dblNumVal = dblNum;
        t->currToken->type = ASM_T_FLOAT;

    } else {
        t->currToken->intNumVal = num;
        t->currToken->type = ASM_T_INTEGER;
    }

    t->column += numLen;
    t->input += numLen;
    return true;
}

static bool checkIdToken(tokenizer_s *t) {
    assert(t != NULL);

    int wordLen = 0;
    sscanf(t->input, "%*[1-9a-zA-Z]%n", &wordLen);

    sscanf(t->input, "%s", t->currToken->val);
    t->currToken->type = ASM_T_ID;


    t->column += wordLen;
    t->input += wordLen;
    return E_ASM_OK;
}


asm_ecode tokenize(tokenizer_s *t) {
    assert(t != NULL);

    asm_ecode err = E_ASM_OK;
    char *textStart = t->input;

    t->currToken = t->tokens;

    for (; *t->input != '\0'; t->currToken++) {
        t->currToken->column = t->column;
        t->currToken->line = t->line;

        size_t tokLen = strspn (t->input, spaces);
        if (tokLen != 0) {
            t->currToken->type = ASM_T_SPACE;
            t->input += tokLen;
            t->column += tokLen;
            continue;
        }


        if (t->currToken->type != ASM_T_EOF)
            continue;

        if (checkLabelToken(t))
            continue;

        if (checkNumberToken(t))
            continue;

        if (checkIdToken(t))
            continue;

        for (size_t i = 0; i < sizeof(generalTokens)/sizeof(generalTokens[0]); i++) {
            if (checkSimpleToken(t, generalTokens[i].type, generalTokens[i].val)) {
                if (generalTokens[i].type == ASM_T_NL) {
                    t->line++;
                    t->column = 1;
                }
                if (generalTokens[i].type == ASM_T_COMMENT) {
                    int len = 0;
                    sscanf(t->input, "%*s%n", &len);
                    t->column += len;
                    t->input += len;
                }
                break;
            }
        }
    }

    t->currToken->type = ASM_T_EOF;
    t->currToken = NULL;

    free(textStart);

    return err;
}


asm_ecode tokenizerInit (tokenizer_s *t, char *input) {
    t->input = input;
    t->column = 1;
    t->line = 1;

    t->currToken = NULL;
    t->tokens = (token_s*)calloc(1024, sizeof(token_s)); // change to list
    if (t->tokens == NULL) {
        perror("asm: ");
        return E_ASM_ERR;
    }

    return E_ASM_OK;

}


token_s *getNextToken (tokenizer_s *t) {
    assert(t != NULL);

    if (t->currToken == NULL){
        t->currToken = t->tokens;
        return t->currToken;
    }

    t->currToken++;
    if (t->currToken->type == ASM_T_EOF)
        return NULL;

    return t->currToken;
}

token_s *peekNextToken (tokenizer_s *t) {
    assert(t != NULL);

    if (t->currToken == NULL){
        t->currToken = t->tokens;
        return t->currToken;
    }

    if (t->currToken->type == ASM_T_EOF)
        return t->currToken;

    return t->currToken + 1;
}

void tokenizerFree (tokenizer_s *t) {
    assert(t != NULL);

    free(t->tokens);
}
