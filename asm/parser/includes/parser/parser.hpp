/** @file */
#ifndef ASM_PARSER_HPP
#define ASM_PARSER_HPP

#include <stddef.h>
#include "symtab.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/errors.hpp"
#include "instructions.hpp"

typedef enum CmdType
{

    CMD_INSTR,
    CMD_DATA_DEF,
    CMD_CONTROL,

} CmdType;

typedef struct commandNode
{
    CmdType Type;

    const char *label;
    const char *name;
    Instruction instr; // TODO change to pointer

    char *data; // data defenition directive data
    size_t dataSz;

    size_t line;
    size_t offset;

} commandNode;

typedef struct sectionNode
{
    const char *name;
    size_t addr;

    commandNode *commands;
    size_t commandsSz;
    size_t commandsCap;

    size_t currOffset; // section size

} sectionNode;

typedef struct Parser
{
    sectionNode *sections;
    size_t sectionsSz;

    sectionNode *currSection;

    symbolTable symTab;

    Tokenizer *toks;

    ParserError *err;
} Parser;

ParserErrCode parseTokens(Parser *p);

int initParser(Parser *p, Tokenizer *toks);

void parserFree(Parser *p);

#endif
