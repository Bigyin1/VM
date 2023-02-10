/** @file */
#ifndef ASM_TOKCHECKERS_PRIVATE_HPP
#define ASM_TOKCHECKERS_PRIVATE_HPP

#include "tokenizer/tokenizer.hpp"

size_t checkLabelDefToken(const char* input, Token* tok);

size_t checkHexNum(const char* input, Token* tok);

size_t checkNumberToken(const char* input, Token* tok);

size_t checkIdToken(const char* input, Token* tok);

size_t checkRegisterToken(const char* input, Token* tok);

size_t checkAsciiCharToken(const char* input, Token* tok);

size_t checkInstrPostfixToken(const char* input, Token* tok);

size_t checkSpaceToken(const char* input, Token* tok);

size_t checkSimpleTokens(const char* input, Token* tok);

#endif
