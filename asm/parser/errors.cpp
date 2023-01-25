#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser/parser.hpp"
#include "errors.hpp"

ParserError *addNewParserError(Parser *p, ParserErrCode eCode)
{
    ParserError *newErr = (ParserError *)calloc(1, sizeof(ParserError)); // TODO check error

    newErr->code = eCode;

    if (p->userErrors == NULL)
    {
        p->userErrors = newErr;
        return newErr;
    }

    ParserError *curr = p->userErrors;
    while (curr->next)
        curr = curr->next;

    curr->next = newErr;
    return newErr;
}

void reportParserErrors(ParserError *err, FILE *f)
{
    assert(err != NULL);
    assert(f != NULL);

    while (err != NULL)
    {
        switch (err->code)
        {
        case PARSER_UNKNOWN_COMMAND:
        {
            fprintf(f, "asm: line: %zu: unknown command: \"%s\"\n",
                    err->line, err->token);
            break;
        }

        case PARSER_COMMAND_INV_ARGS:
        {
            fprintf(f, "asm: line: %zu: invalid arguments for command: \"%s\"\n",
                    err->line, err->token);
            break;
        }
        case PARSER_UNKNOWN_REGISTER:
        {
            fprintf(f, "asm: line: %zu column: %zu: unknown register \"%s\"\n",
                    err->line, err->column, err->token);
            break;
        }
        case PARSER_BAD_CMD_POSTFIX:
        {
            fprintf(f, "asm: line %zu: command has invalid postfix \"%s\" (column: %zu)\n",
                    err->line, err->token, err->column);
            break;
        }

        case PARSER_LABEL_REDEF:
        {
            fprintf(f, "asm: line %zu: label \"%s\" defined twice\n", err->line, err->token);
            break;
        }

        case PARSER_LABEL_UNDEFIEND:
        {
            fprintf(f, "asm: label \"%s\" was not defined\n",
                    err->token);
            break;
        }

        case PARSER_INSUFF_TOKEN:
        {
            fprintf(f, "asm: line: %zu column: %zu: got: %s  expected: %s\n",
                    err->line, err->column,
                    tokenTypeVerbose(err->got), tokenTypeVerbose(err->expected));
            break;
        }
        default:
            break;
        }

        err = err->next;
    }
}
