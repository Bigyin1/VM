/** @file */
#ifndef ASM_PARSER_UTILS_HPP
#define ASM_PARSER_UTILS_HPP

#include <stddef.h>
#include "parser/parser.hpp"

ParserErrCode eatToken(Parser *p, TokenType type);

void eatNL(Parser *p);

void eatSP(Parser *p);

void eatBlanks(Parser *p);

void eatUntillNL(Parser *p);

DataSize evalImmMinDataSz(uint64_t val, TokenType type);

#endif
