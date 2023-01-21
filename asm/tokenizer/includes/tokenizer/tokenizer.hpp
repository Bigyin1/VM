/** @file */
#ifndef ASM_TOKENIZER_PUBLIC_HPP
#define ASM_TOKENIZER_PUBLIC_HPP

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "tokenizer/errors.hpp"

enum TokenType
{
    ASM_T_EOF,
    ASM_T_ID,
    ASM_T_FLOAT,
    ASM_T_SIGNED_INT,
    ASM_T_UNSIGNED_INT,
    ASM_T_LABEL,
    ASM_T_SPACE,

    ASM_T_SECTION_NAME,
    ASM_T_COMMA,
    ASM_T_PLUS,
    ASM_T_MINUS,
    ASM_T_L_PAREN,
    ASM_T_R_PAREN,
    ASM_T_L_SIMP_PAREN,
    ASM_T_R_SIMP_PAREN,
    ASM_T_NL,
    ASM_T_COMMENT,

};

const size_t maxTokenValLen = 24;

struct Token
{
    char val[maxTokenValLen + 1];
    int64_t numVal;

    TokenType type;

    size_t line;
    size_t column;
};

typedef struct Token Token;

struct Tokenizer
{
    Token *tokens;
    Token *currToken;

    Token *saved;

    char *input;
    size_t line;
    size_t column;

    TokenizerError *err;
};

typedef struct Tokenizer Tokenizer;

int tokenizerInit(Tokenizer *t, char *input);

int Tokenize(Tokenizer *t);

Token *getNextToken(Tokenizer *t);

Token *peekNextToken(Tokenizer *t);

Token *saveCurrToken(Tokenizer *t);

Token *restoreSavedToken(Tokenizer *t);

void tokenizerFree(Tokenizer *t);

void tokenizerDump(Tokenizer *t, FILE *out);

const char *tokenTypeVerbose(TokenType t);

#endif
