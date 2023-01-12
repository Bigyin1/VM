/** @file */
#ifndef ASM_TOK_HPP
#define ASM_TOK_HPP

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "../errors.hpp"

enum e_asm_token_type
{
  ASM_T_EOF,
  ASM_T_ID,
  ASM_T_FLOAT,
  ASM_T_SIGNED_INT,
  ASM_T_UNSIGNED_INT,
  ASM_T_LABEL,
  ASM_T_SPACE,

  ASM_T_SECTION_NAME,
  ASM_T_COMMA,
  ASM_T_PLUS,
  ASM_T_MINUS,
  ASM_T_L_PAREN,
  ASM_T_R_PAREN,
  ASM_T_L_SIMP_PAREN,
  ASM_T_R_SIMP_PAREN,
  ASM_T_NL,
  ASM_T_COMMENT,

};

typedef enum e_asm_token_type e_asm_token_type;

typedef struct token_meta_s
{

  e_asm_token_type type;
  const char *val;

} token_meta_s;

const size_t maxTokenValLen = 24;

struct token_s
{
  char val[maxTokenValLen + 1]; // polymorf
  double dblNumVal;             // TODO: remove this field
  int64_t intNumVal;

  e_asm_token_type type;

  size_t line;
  size_t column;
};

typedef struct token_s token_s;

struct tokenizer_s
{
  token_s *tokens;
  token_s *currToken;

  token_s *saved;

  char *input;
  size_t line;
  size_t column;
};

typedef struct tokenizer_s tokenizer_s;

asm_ecode tokenizerInit(tokenizer_s *t, char *input);

asm_ecode tokenize(tokenizer_s *t);

token_s *getNextToken(tokenizer_s *t);

token_s *peekNextToken(tokenizer_s *t);

token_s *saveCurrToken(tokenizer_s *t);

token_s *restoreSavedToken(tokenizer_s *t);

void tokenizerFree(tokenizer_s *t);

void tokenizerDump(tokenizer_s *t, FILE *out);

#endif
