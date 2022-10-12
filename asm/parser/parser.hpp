/** @file */
#ifndef ASM_PARSER_HPP
#define ASM_PARSER_HPP

#include <stddef.h>
#include "../errors.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "../../vm/cpu/instructions.hpp"


typedef struct dataNode {
    size_t addr;
    char data[128];

} dataNode;



typedef struct instrArgument {
    char *val;
    short encVal;
    e_arg_type type;

    bool isAddr;

} instrArgument;


typedef struct commandNode {
    char *label;

    size_t offset;
    const char *instrName;

    instrArgument args[instrArgsMaxCount];

    size_t line;

} commandNode;


typedef struct codeNode {
    size_t addr;
    commandNode commands[256];

} codeNode;


typedef struct programNode {
    codeNode code;
    dataNode data;

} programNode;




#define currTokenType(p) (p)->toks->currToken->type

#define currTokenVal(p) (p)->toks->currToken->val;

typedef struct parser_s {
    programNode prog;
    tokenizer_s *toks;

} parser_s;


asm_ecode parseTokens(parser_s *p);


#endif
