/** @file */
#ifndef ASM_TOK_HPP
#define ASM_TOK_HPP

#include <stddef.h>
#include <stdio.h>
#include "../errors.hpp"

enum e_asm_token_type
{
  ASM_T_EOF,
  ASM_T_WORD,
  ASM_T_COLON,
  ASM_T_L_PAREN,
  ASM_T_R_PAREN,
  ASM_T_SPACE,
  ASM_T_NL,
  ASM_T_COMMA,
  ASM_T_COMMENT,
  ASM_T_DOT,
};

typedef enum e_asm_token_type e_asm_token_type;


#define TOK_COLON ":"
#define TOK_COMMA ","
#define TOK_L_PAREN "["
#define TOK_R_PAREN "]"
#define TOK_SPACE " "
#define TOK_NL "\n"
#define TOK_COMMENT "#"
#define TOK_DOT "."




struct token_s
{
    char *val;
    e_asm_token_type type;
    size_t line;
    size_t column;
};

typedef struct token_s token_s;



struct tokenizer_s
{
    token_s tokens[1024];
    token_s *currToken;
};

typedef struct tokenizer_s tokenizer_s;




asm_ecode tokenize (tokenizer_s *t, char *input);

token_s *getNextToken (tokenizer_s *t);

token_s *peekNextToken (tokenizer_s *t);

void tokenizerFree (tokenizer_s *t);

void tokenizerDump(tokenizer_s *t, FILE *out);




#endif
