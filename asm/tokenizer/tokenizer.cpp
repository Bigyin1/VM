#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tokenizer.hpp"

const char *spaces = " \t";
const char *reservedSymbols = " \t\n[]:#,";

static bool checkSimpleToken(char **input, token_s *tok, size_t *column, e_asm_token_type type, const char *tokVal) {
    assert(input != NULL);
    assert(*input != NULL);
    assert(column != NULL);
    assert(tok != NULL);
    assert(tokVal != NULL);

    size_t tokLen = strlen (tokVal);
    if (strncmp (*input, tokVal, tokLen) != 0)
        return false;

    tok->type = type;
    tok->val = strdup(tokVal);

    *column += tokLen;
    *input += tokLen;
    return true;
}


static asm_ecode checkWordToken(char **input, token_s *tok, size_t *column) {
    assert(input != NULL);
    assert(*input != NULL);
    assert(column != NULL);
    assert(tok != NULL);

    size_t wordLen = strcspn (*input, reservedSymbols);

    tok->val = (char *)calloc(wordLen + 1, sizeof(char));
    if (tok->val == NULL) {
        perror("asm");
        return E_ASM_ERR;
    }
    tok->type = ASM_T_WORD;

    strncpy(tok->val, *input, wordLen);
    tok->val[wordLen] = '\0';

    *column += wordLen;
    *input += wordLen;
    return E_ASM_OK;
}


asm_ecode tokenize (tokenizer_s *t, char *input) {
    assert(t != NULL);
    assert(input != NULL);


    char *textStart = input;
    token_s *tok = t->tokens;
    size_t tokLen = 0;
    size_t line = 0;
    size_t column = 1;


    for (; *input != '\0'; tok++) {
        tok->column = column;
        tok->line = line;

        tokLen = strspn (input, spaces);
        if (tokLen != 0) {
            tok->type = ASM_T_SPACE;
            tok->val = strdup(TOK_SPACE);
            input += tokLen;
            column += tokLen;
            continue;
        }

        if (checkSimpleToken(&input, tok, &column, ASM_T_NL, TOK_NL)) {
            line++;
            column = 1;
            continue;
        }
        if (checkSimpleToken(&input, tok, &column, ASM_T_COMMA, TOK_COMMA))
            continue;
        if (checkSimpleToken(&input, tok, &column, ASM_T_L_PAREN, TOK_L_PAREN))
            continue;
        if (checkSimpleToken(&input, tok, &column, ASM_T_R_PAREN, TOK_R_PAREN))
            continue;
        if (checkSimpleToken(&input, tok, &column, ASM_T_COLON, TOK_COLON))
            continue;
        if (checkSimpleToken(&input, tok, &column, ASM_T_DOT, TOK_DOT))
            continue;


        if (checkWordToken(&input, tok, &column) != E_ASM_OK)
            return E_ASM_ERR;
    }

    tok->val = NULL;
    tok->type = ASM_T_EOF;

    free(textStart);

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

    if ((t->currToken + 1)->type == ASM_T_EOF)
        return NULL;

    return t->currToken + 1;
}


void tokenizerDump(tokenizer_s *t, FILE *out) {
    assert(t != NULL);
    assert(out != NULL);

    token_s *tok = t->tokens;

    while (tok->type != ASM_T_EOF) {
        fprintf(out, "tok(%zu %zu %s) ", tok->line, tok->column, tok->val);
        tok++;
    }
    fprintf(out, "\n");
}

void tokenizerFree (tokenizer_s *t) {
    assert(t != NULL);

    token_s *tok = t->tokens;
    while (tok->type != ASM_T_EOF) {
        free(tok->val);
        tok++;
    }

    // free(t->tokens); - TODO
}
