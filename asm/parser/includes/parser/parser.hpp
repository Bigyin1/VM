/** @file */
#ifndef ASM_PARSER_HPP
#define ASM_PARSER_HPP

#include <stddef.h>

#include "instructions.hpp"
#include "parser/errors.hpp"
#include "symtab.hpp"
#include "tokenizer/tokenizer.hpp"

typedef enum CmdType
{

    CMD_INSTR,
    CMD_DATA_DEF,
    CMD_CONTROL,

} CmdType;

typedef struct CommandNode
{
    CmdType Type;

    const char* label;
    const char* name;

    Instruction instr;

    char*  data; // data defenition directive data
    size_t dataSz;

    size_t line;

    size_t offset; // instr offset in section

} CommandNode;

typedef struct SectionNode
{
    const char* name;

    CommandNode* commands;
    size_t       commandsSz;
    size_t       commandsCap;

    size_t size; // section size

} SectionNode;

typedef struct Parser
{
    SectionNode* sections;
    size_t       sectionsSz;

    SectionNode* currSection;

    symbolsData symsData;

    Tokenizer* toks;

    ParserError* userErrors;
} Parser;

ParserErrCode ParseTokens(Parser* p);

ParserErrCode ParserInit(Parser* p, Tokenizer* toks);

void parserFree(Parser* p);

#endif
