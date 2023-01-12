/** @file */
#ifndef ASM_DIRECTIVES_HPP
#define ASM_DIRECTIVES_HPP

#include "parser.hpp"

static asm_ecode parseDataDefDirective(parser_s *parser, commandNode *node);

static asm_ecode parseControlDirective(parser_s *parser, commandNode *node);

#endif
