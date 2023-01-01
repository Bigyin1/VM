/** @file */
#ifndef ASM_ERRORS_HPP
#define ASM_ERRORS_HPP

#define PARSE_ERROR "asm: parsing error at line: %zu, column: %zu\n"


typedef enum e_asm_codes
{
  E_ASM_OK = 0,
  E_ASM_ERR = 1,
  E_ASM_INSUFF_TOKEN = 2,
} asm_ecode;

#endif
