/** @file */
#ifndef ASM_PARSER_ERRORS_HPP
#define ASM_PARSER_ERRORS_HPP

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "tokenizer/tokenizer.hpp"

typedef enum ParserErrCode
{
    PARSER_OK,
    PARSER_SYSTEM_ERR,
    PARSER_BAD_COMMAND,

    PARSER_UNKNOWN_COMMAND,
    PARSER_COMMAND_INV_ARGS,
    PARSER_UNKNOWN_REGISTER,
    PARSER_BAD_CMD_POSTFIX,
    PARSER_LABEL_REDEF,
    PARSER_LABEL_UNDEFIEND,
    PARSER_INSUFF_TOKEN,

} ParserErrCode;

typedef struct ParserError
{
    ParserError *next;

    size_t line;
    size_t column;

    const char *token;

    TokenType got;
    TokenType expected;

    ParserErrCode code;

} ParserError;

void reportParserErrors(ParserError *err, FILE *f);

#endif
