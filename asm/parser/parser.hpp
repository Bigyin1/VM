/** @file */
#ifndef ASM_PARSER_HPP
#define ASM_PARSER_HPP

#include <stddef.h>
#include "../errors.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "../../vm/instructions/instruction.hpp"




typedef struct commandNode {
    const char *label;
    const char *instrName;
    Instruction instr;


    size_t line;
    unsigned int offset;

} commandNode;


typedef struct codeNode {
    size_t addr;
    commandNode commands[256];

} codeNode;


typedef struct labelData
{
    const char *label;
    uint64_t    val;

    uint64_t    *imports[16];
    size_t      importsSz;

    bool        present;

} labelData;



typedef struct parser_s {
    codeNode    prog;
    tokenizer_s *toks;

    labelData   labels[64];
    size_t      labelsSz;

} parser_s;


#define currTokenType(p) (p)->toks->currToken->type

#define currTokenVal(p) (p)->toks->currToken->val;

#define currTokenDblNumVal(p) (p)->toks->currToken->dblNumVal;

#define currTokenIntNumVal(p) (p)->toks->currToken->intNumVal;



asm_ecode parseTokens(parser_s *p);


#endif
