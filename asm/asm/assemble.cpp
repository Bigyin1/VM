#include <string.h>
#include "utils.hpp"
#include "assemble/assemble.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"
#include "asmencoder/asmencoder.hpp"

AsmErrCode assemble(FILE *in, FILE *out)
{

    char *text = readFile(in);
    if (text == NULL)
        return ASM_SYSTEM_ERROR;

    Tokenizer tokenizer = {0};
    if (TokenizerInit(&tokenizer, text) != TOK_OK)
        return ASM_SYSTEM_ERROR;

    if (Tokenize(&tokenizer) != TOK_OK)
        return ASM_SYSTEM_ERROR;

    if (tokenizer.userErrors != NULL)
    {
        reportErrors(tokenizer.userErrors, stderr);
        tokenizerFree(&tokenizer);
        return ASM_USER_ERROR;
    }

    Parser parser = {0};
    if (ParserInit(&parser, &tokenizer) != PARSER_OK)
        return ASM_SYSTEM_ERROR;

    if (ParseTokens(&parser) != PARSER_OK)
        return ASM_SYSTEM_ERROR;

    if (parser.userErrors != NULL)
    {
        reportParserErrors(parser.userErrors, stderr);
        tokenizerFree(&tokenizer);
        parserFree(&parser);
        return ASM_USER_ERROR;
    }

    AsmEncoder as = {.parser = &parser, .out = out};
    EncErrCode err = GenObjectFile(&as);

    fclose(out);
    tokenizerFree(&tokenizer);
    parserFree(&parser);

    if (err == ENC_USER_ERROR)
        return ASM_USER_ERROR;

    if (err == ENC_SYSTEM_ERROR)
        return ASM_SYSTEM_ERROR;

    return ASM_OK;
}
