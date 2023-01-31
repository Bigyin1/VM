/** @file */
#ifndef ASM_LABELS_HPP
#define ASM_LABELS_HPP

#include "parser/parser.hpp"

ParserErrCode defineNewSymbol(Parser *p, const char *name, uint64_t val);

ParserErrCode defineNewAbsSymbol(Parser *p, const char *name, uint64_t val);

ParserErrCode addSymbolReference(Parser *p, const char *name, uint64_t val);

#endif
