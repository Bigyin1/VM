#include <stdio.h>
#include <string.h>
#include "utils.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"
#include "assembler/assembler.hpp"

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("Wrong args count\n");
        return 1;
    }

    char *text = readFile(argv[1]);
    if (text == NULL)
    {
        perror(argv[1]);
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (out == NULL)
    {
        perror(argv[2]);
        return 1;
    }

    tokenizer_s tokenizer = {0};
    if (tokenizerInit(&tokenizer, text) == E_ASM_ERR)
        return 1;

    if (tokenize(&tokenizer) == E_ASM_ERR)
        return 1;

    parser_s parser = {.prog = 0, .toks = &tokenizer};
    if (parseTokens(&parser) == E_ASM_ERR)
        return 1;

    // assembler_s as = {.prog = &parser.prog, .out = out};
    // assemble(&as);
    fclose(out);

    tokenizerFree(&tokenizer);
}
