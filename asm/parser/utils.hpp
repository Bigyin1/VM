/** @file */
#ifndef ASM_PARSER_UTILS_HPP
#define ASM_PARSER_UTILS_HPP

#include <stddef.h>

#include "parser/parser.hpp"

ParserErrCode eatToken(Parser* p, TokenType type);

void eatNL(Parser* p);

void eatSP(Parser* p);

void eatBlanks(Parser* p);

void eatUntillNL(Parser* p);

DataSize evalImmMinDataSz(int64_t val);

#define currTokenType(p)   (p)->toks->currToken->type

#define currTokenVal(p)    (p)->toks->currToken->val

#define currTokenNumVal(p) (p)->toks->currToken->numVal

#define currTokenLine(p)   (p)->toks->currToken->line

#define currTokenColumn(p) (p)->toks->currToken->column

#endif
