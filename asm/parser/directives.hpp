/** @file */
#ifndef ASM_DIRECTIVES_HPP
#define ASM_DIRECTIVES_HPP

#include "parser/parser.hpp"

ParserErrCode parseDataDefDirective(Parser* parser, CommandNode* node);

ParserErrCode parseControlDirective(Parser* parser, CommandNode* node);

#endif
