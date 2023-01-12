/** @file */
#ifndef ASM_PARSER_HPP
#define ASM_PARSER_HPP

#include <stddef.h>
#include "../errors.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "../../vm/instructions/instructions.hpp"

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

typedef struct labelData
{
    const char *label;
    uint64_t val;

    uint64_t *imports[16];
    size_t importsSz;

    bool present;

} labelData;

typedef struct sectionNode
{
    const char *name;
    size_t addr;

    commandNode *commands;
    size_t commandsSz;
    size_t commandsCap;

    size_t currOffset; // section size

} sectionNode;

typedef struct parser_s
{
    sectionNode *sections;
    size_t sectionsSz;

    sectionNode *currSection;

    labelData labels[64];
    size_t labelsSz;

    tokenizer_s *toks;
} parser_s;

#define currTokenType(p) (p)->toks->currToken->type

#define currTokenVal(p) (p)->toks->currToken->val

#define currTokenDblNumVal(p) (p)->toks->currToken->dblNumVal

#define currTokenIntNumVal(p) (p)->toks->currToken->intNumVal

asm_ecode parseTokens(parser_s *p);

asm_ecode initParser(parser_s *p, tokenizer_s *toks);

void parserFree(parser_s *p);

#endif
