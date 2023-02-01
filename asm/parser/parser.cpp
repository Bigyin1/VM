#include <assert.h>
#include <string.h>
#include <math.h>
#include "parser/parser.hpp"
#include "section_parser.hpp"
#include "utils.hpp"
#include "symbols.hpp"

static ParserErrCode reallocSections(Parser *parser)
{
    if (parser->sectionsSz == 0)
    {
        parser->sections = (sectionNode *)calloc(++parser->sectionsSz, sizeof(sectionNode));
        return PARSER_OK;
    }

    size_t newSz = parser->sectionsSz + 1;
    sectionNode *newSections = (sectionNode *)calloc(newSz, sizeof(sectionNode));

    if (newSections == NULL)
        return PARSER_SYSTEM_ERR;

    memcpy(newSections, parser->sections, parser->sectionsSz * sizeof(sectionNode));
    free(parser->sections);

    parser->sectionsSz = newSz;
    parser->sections = newSections;
    return PARSER_OK;
}

ParserErrCode ParseTokens(Parser *parser)
{
    assert(parser != NULL);

    getNextToken(parser->toks);

    while (1)
    {
        eatBlanks(parser);
        if (currTokenType(parser) == ASM_T_EOF)
            break;

        const char *sectName = currTokenVal(parser);

        eatToken(parser, ASM_T_SECTION_NAME); // TODO: check for duplicating sections

        ParserErrCode err = reallocSections(parser);
        if (err != PARSER_OK)
            return err;

        parser->currSection = parser->sections + parser->sectionsSz - 1;
        parser->currSection->name = sectName;

        eatSP(parser);

        err = parseSectionNode(parser, parser->currSection);
        if (err != PARSER_OK)
            return err;
    }

    return PARSER_OK;
}

ParserErrCode ParserInit(Parser *p, Tokenizer *toks)
{
    assert(p != NULL);

    p->toks = toks;

    return PARSER_OK;
}

void parserFree(Parser *p)
{
    for (size_t i = 0; i < p->sectionsSz; i++)
    {
        sectionNode *currSect = p->sections + i;

        for (size_t j = 0; j < currSect->commandsSz; j++)
            if (currSect->commands[j].dataSz > 0)
                free(currSect->commands[j].data);

        free(currSect->commands);
    }

    while (p->userErrors != NULL)
    {
        ParserError *err = p->userErrors;
        p->userErrors = p->userErrors->next;

        free(err);
    }

    free(p->sections);
}
