/** @file */
#ifndef ASM_PARSER_UTILS_HPP
#define ASM_PARSER_UTILS_HPP

#include "../errors.hpp"
#include "parser.hpp"

asm_ecode eatToken (parser_s *p, e_asm_token_type type);

void eatNL(parser_s *p);

void eatSP(parser_s *p);

void eatBlanks(parser_s *p);

#endif
