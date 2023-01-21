/** @file */
#ifndef ASM_DIRECTIVES_HPP
#define ASM_DIRECTIVES_HPP

#include "parser/parser.hpp"

ParserErrCode parseDataDefDirective(Parser *parser, commandNode *node);

ParserErrCode parseControlDirective(Parser *parser, commandNode *node);

#endif
