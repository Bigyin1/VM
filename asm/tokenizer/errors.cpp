#include "errors.hpp"

#include <assert.h>
#include <stdlib.h>

#include "tokenizer/errors.hpp"

int addUnknownTokenError(Tokenizer* t)
{
    TokenizerError* newErr = (TokenizerError*)calloc(1, sizeof(TokenizerError));
    if (newErr == NULL)
        return -1;

    char* text = (char*)calloc(MAX_TOKEN_LEN + 1, sizeof(char));
    if (text == NULL)
    {
        free(newErr);
        return -1;
    }

    newErr->code   = TOK_UNKNOWN;
    newErr->column = t->column;
    newErr->line   = t->line;
    newErr->text   = text;

    unsigned wordLen = 0;
    sscanf(t->input, "%" XSTR(MAX_TOKEN_LEN) "s%n", newErr->text, (int*)&wordLen);

    t->input += wordLen;
    t->column += wordLen;

    if (t->userErrors == NULL)
    {
        t->userErrors = newErr;
        return 0;
    }

    TokenizerError* curr = t->userErrors;
    while (curr->next)
        curr = curr->next;

    curr->next = newErr;
    return 0;
}

void reportErrors(TokenizerError* err, FILE* f)
{
    assert(err != NULL);
    assert(f != NULL);

    while (err != NULL)
    {
        if (err->code == TOK_UNKNOWN)
        {
            fprintf(f, "asm: unknown token \"%s\" at line: %zu; column: %zu\n", err->text,
                    err->line, err->column);
        }

        err = err->next;
    }
}
