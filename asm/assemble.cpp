#include <string.h>
#include "utils.hpp"
#include "assemble.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"
#include "assembler/assembler.hpp"

e_asm_codes assemble(FILE *in, FILE *out)
{

    char *text = readFile(in);
    if (text == NULL)
    {
        perror("");
        return E_ASM_ERR;
    }
    fclose(in);

    tokenizer_s tokenizer = {0};
    if (tokenizerInit(&tokenizer, text) == E_ASM_ERR)
        return E_ASM_ERR;

    if (tokenize(&tokenizer) == E_ASM_ERR)
        return E_ASM_ERR;

    parser_s parser = {.prog = 0, .toks = &tokenizer};
    if (parseTokens(&parser) == E_ASM_ERR)
        return E_ASM_ERR;

    assembler_s as = {.prog = &parser.prog, .out = out};
    if (assemble(&as) == E_ASM_ERR)
        return E_ASM_ERR;

    fclose(out);

    tokenizerFree(&tokenizer);

    return E_ASM_OK;
}
