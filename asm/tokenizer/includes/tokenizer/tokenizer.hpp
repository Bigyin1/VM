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
    ASM_T_INT,
    ASM_T_LABEL_DEF,
    ASM_T_REGISTER,
    ASM_T_INSTR_POSTFIX,
    ASM_T_SPACE,

    ASM_T_SECTION,
    ASM_T_COMMA,
    ASM_T_PLUS,
    ASM_T_MINUS,
    ASM_T_L_PAREN,
    ASM_T_R_PAREN,
    ASM_T_NL,
    ASM_T_COMMENT,

};

#define MAX_TOKEN_LEN 24

#define STR(s) #s
#define XSTR(s) STR(s)

struct Token
{
    char val[MAX_TOKEN_LEN + 1];
    int64_t numVal;

    TokenType type;

    size_t line;
    size_t column;
};

typedef struct Token Token;

struct Tokenizer
{
    Token *tokens;
    size_t tokensSz;
    size_t tokensCap;

    Token *currToken;

    Token *saved;

    char *input;
    size_t line;
    size_t column;

    TokenizerError *userErrors;
};

typedef struct Tokenizer Tokenizer;

TokErrCode TokenizerInit(Tokenizer *t, char *input);

TokErrCode Tokenize(Tokenizer *t);

Token *getNextToken(Tokenizer *t);

Token *peekNextToken(Tokenizer *t);

Token *saveCurrToken(Tokenizer *t);

Token *restoreSavedToken(Tokenizer *t);

void tokenizerFree(Tokenizer *t);

void tokenizerDump(Tokenizer *t, FILE *out);

const char *tokenTypeVerbose(TokenType t);

#endif
