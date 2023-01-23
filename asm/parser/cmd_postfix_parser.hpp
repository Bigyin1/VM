/** @file */
#ifndef ASM_COMMAND_POSTFIX_PARSER_HPP
#define ASM_COMMAND_POSTFIX_PARSER_HPP

#include "parser/parser.hpp"

ParserErrCode parseInstrPostfix(Parser *parser, commandNode *node);

#endif
