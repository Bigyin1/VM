#include <stdio.h>
#include "utils.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"


int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong args count\n");
        return 1;
    }

    char *text = readFile(argv[1]);
    if (text == NULL) {
        perror(argv[1]);
        return 1;
    }

    tokenizer_s tokenizer = {0};

    tokenize(&tokenizer, text);
    #ifdef _DEBUG
    tokenizerDump(&tokenizer, stdout);
    #endif

    parser_s parser = {.prog = 0, .toks = &tokenizer};
    parseTokens(&parser);

    tokenizerFree(&tokenizer);
}
