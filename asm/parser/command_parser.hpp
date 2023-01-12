/** @file */
#ifndef ASM_COMMAND_PARSER_HPP
#define ASM_COMMAND_PARSER_HPP

#include "../errors.hpp"
#include "parser.hpp"

asm_ecode parseCommandNode(parser_s *parser, commandNode *node);

#endif
