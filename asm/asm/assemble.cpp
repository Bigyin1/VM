#include <string.h>
#include "utils.hpp"
#include "assemble.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"
#include "encoder.hpp"

int assemble(FILE *in, FILE *out)
{

    char *text = readFile(in);
    if (text == NULL)
        return -1;

    Tokenizer tokenizer = {0};
    if (tokenizerInit(&tokenizer, text) < 0)
        return -1;

    if (Tokenize(&tokenizer) < 0)
        return -1;

    if (tokenizer.err != NULL)
    {
        reportErrors(tokenizer.err, stderr);
        tokenizerFree(&tokenizer);
        return 0;
    }

    Parser parser = {0};
    if (initParser(&parser, &tokenizer) < 0)
        return -1;

    ParserErrCode err = parseTokens(&parser);
    if (err == PARSER_SYSTEM_ERR)
    {
        perror("asm:");
        if (parser.err != NULL)
            reportParserErrors(parser.err, stderr);
        tokenizerFree(&tokenizer);
        parserFree(&parser);
        return -1;
    }
    else if (parser.err != NULL)
    {
        reportParserErrors(parser.err, stderr);
        tokenizerFree(&tokenizer);
        parserFree(&parser);
        return 0;
    }

    AsmEncoder as = {.parser = &parser, .out = out};
    if (Encode(&as) == E_ASM_ERR)
        return E_ASM_ERR;

    fclose(out);

    tokenizerFree(&tokenizer);
    parserFree(&parser);

    return 0;
}
