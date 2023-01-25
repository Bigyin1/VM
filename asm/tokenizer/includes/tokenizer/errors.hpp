/** @file */
#ifndef ASM_TOKENIZER_ERRORS_HPP
#define ASM_TOKENIZER_ERRORS_HPP

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum TokErrCode
{
    TOK_UNKNOWN,

    TOK_SYTEM_ERROR,
    TOK_OK,
} TokErrCode;

typedef struct TokenizerError
{
    TokenizerError *next;

    size_t line;
    size_t column;

    char *text;

    TokErrCode code;

} TokenizerError;

void reportErrors(TokenizerError *err, FILE *f);

#endif
