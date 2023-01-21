/** @file */
#ifndef ASM_LABELS_HPP
#define ASM_LABELS_HPP

#include "parser/parser.hpp"

ParserErrCode defineNewLabel(Parser *p, const char *label, uint64_t val);

void addLabelImport(Parser *p, const char *label, uint64_t *v);

ParserErrCode resolveImports(Parser *p);

#endif
