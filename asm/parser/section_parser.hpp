/** @file */
#ifndef ASM_SECTION_PARSER_HPP
#define ASM_SECTION_PARSER_HPP

#include "../errors.hpp"
#include "parser.hpp"

asm_ecode parseSectionNode(parser_s *parser, sectionNode *sectNode);

#endif
