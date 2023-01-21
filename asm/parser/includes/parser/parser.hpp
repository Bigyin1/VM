/** @file */
#ifndef ASM_PARSER_HPP
#define ASM_PARSER_HPP

#include <stddef.h>
#include "tokenizer/tokenizer.hpp"
#include "parser/errors.hpp"
#include "instructions.hpp"

typedef struct labelData
{
    const char *label;
    uint64_t val;

    uint64_t *imports[16];
    size_t importsSz;

    bool present;

} labelData;

typedef enum CmdType
{

    CMD_INSTR,
    CMD_DATA_DEF,
    // CMD_EQU,

} CmdType;

typedef struct commandNode
{
    CmdType Type;

    const char *label;
    const char *name;
    Instruction instr;

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

    labelData labels[64];
    size_t labelsSz;

    Tokenizer *toks;

    ParserError *err;
} Parser;

#define currTokenType(p) (p)->toks->currToken->type

#define currTokenVal(p) (p)->toks->currToken->val

#define currTokenNumVal(p) (p)->toks->currToken->numVal

#define currTokenLine(p) (p)->toks->currToken->line

#define currTokenColumn(p) (p)->toks->currToken->column

ParserErrCode parseTokens(Parser *p);

int initParser(Parser *p, Tokenizer *toks);

void parserFree(Parser *p);

#endif
