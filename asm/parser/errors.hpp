/** @file */
#ifndef ASM_PARSER_ERRORS_PRIVATE_HPP
#define ASM_PARSER_ERRORS_PRIVATE_HPP

#include "parser/parser.hpp"

ParserError* newParserError(Parser* p);

void addUnknownCommandError(Parser* p, CommandNode* node);

void addCommandInvArgsError(Parser* p, const char* cmdName, size_t line);

void addUnknownRegError(Parser* p);

void addBadInstrPostfixError(Parser* p, const char* pfix);

void addLabelRedefError(Parser* p, const char* label);

#endif
