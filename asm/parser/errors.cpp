#include "errors.hpp"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.hpp"
#include "utils.hpp"

ParserError* newParserError(Parser* p)
{
    ParserError* newErr = (ParserError*)calloc(1, sizeof(ParserError));

    if (p->userErrors == NULL)
    {
        p->userErrors = newErr;
        return newErr;
    }

    ParserError* curr = p->userErrors;
    while (curr->next)
        curr = curr->next;

    curr->next = newErr;
    return newErr;
}

void addUnknownCommandError(Parser* p, CommandNode* node)
{

    ParserError* err = newParserError(p);

    err->code = PARSER_UNKNOWN_COMMAND;

    err->token = node->name;
    err->line  = node->line;
}

void addCommandInvArgsError(Parser* p, const char* cmdName, size_t line)
{

    ParserError* err = newParserError(p);

    err->code = PARSER_COMMAND_INV_ARGS;

    err->token = cmdName;
    err->line  = line;
}

void addUnknownRegError(Parser* p)
{

    ParserError* err = newParserError(p);

    err->code = PARSER_UNKNOWN_REGISTER;

    err->token  = currTokenVal(p);
    err->line   = currTokenLine(p);
    err->column = currTokenColumn(p);
}

void addBadInstrPostfixError(Parser* p, const char* pfix)
{

    ParserError* err = newParserError(p);

    err->code  = PARSER_UNKNOWN_REGISTER;
    err->token = pfix;
    err->line  = currTokenLine(p);
}

void addLabelRedefError(Parser* p, const char* label)
{

    ParserError* err = newParserError(p);

    err->code = PARSER_LABEL_REDEF;

    err->token = label;
    err->line  = currTokenLine(p);
}

void reportParserErrors(ParserError* err, FILE* f)
{
    assert(err != NULL);
    assert(f != NULL);

    while (err != NULL)
    {
        switch (err->code)
        {
            case PARSER_UNKNOWN_COMMAND:
            {
                fprintf(f, "asm: line: %zu: unknown command: \"%s\"\n", err->line, err->token);
                break;
            }

            case PARSER_COMMAND_INV_ARGS:
            {
                fprintf(f, "asm: line: %zu: invalid arguments for command: \"%s\"\n", err->line,
                        err->token);
                break;
            }
            case PARSER_UNKNOWN_REGISTER:
            {
                fprintf(f, "asm: line: %zu column: %zu: unknown register \"%s\"\n", err->line,
                        err->column, err->token);
                break;
            }
            case PARSER_BAD_CMD_POSTFIX:
            {
                fprintf(f, "asm: line %zu: command has invalid postfix \"%s\"\n", err->line,
                        err->token);
                break;
            }
            case PARSER_LABEL_REDEF:
            {
                fprintf(f, "asm: line %zu: label \"%s\" defined twice\n", err->line, err->token);
                break;
            }
            case PARSER_LABEL_UNDEFIEND:
            {
                fprintf(f, "asm: label \"%s\" was not defined\n", err->token);
                break;
            }
            case PARSER_INSUFF_TOKEN:
            {
                fprintf(f, "asm: line: %zu column: %zu: got: %s  expected: %s\n", err->line,
                        err->column, TokenTypeVerbose(err->got), TokenTypeVerbose(err->expected));
                break;
            }
            default:
                break;
        }

        err = err->next;
    }
}
