/** @file */
#ifndef ASM_DIRECTIVES_HPP
#define ASM_DIRECTIVES_HPP

#include "parser.hpp"

asm_ecode parseDataDefDirective(parser_s *parser, commandNode *node);

asm_ecode parseControlDirective(parser_s *parser, commandNode *node);

#endif
