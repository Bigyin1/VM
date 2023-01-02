/** @file */
#ifndef ASM_LABELS_HPP
#define ASM_LABELS_HPP

#include "parser.hpp"

int defineNewLabel(parser_s *p, const char *label, uint64_t val);

void addLabelImport(parser_s *p, const char *label, uint64_t *v);

#endif
