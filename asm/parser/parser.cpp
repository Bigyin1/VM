#include "parser/parser.hpp"

#include <assert.h>
#include <math.h>
#include <string.h>

#include "section_parser.hpp"
#include "symbols.hpp"
#include "utils.hpp"

static ParserErrCode reallocSections(Parser* parser)
{
    if (parser->sectionsSz == 0)
    {
        parser->sections = (SectionNode*)calloc(++parser->sectionsSz, sizeof(SectionNode));
        return PARSER_OK;
    }

    size_t       newSz       = parser->sectionsSz + 1;
    SectionNode* newSections = (SectionNode*)calloc(newSz, sizeof(SectionNode));

    if (newSections == NULL)
        return PARSER_SYSTEM_ERR;

    memcpy(newSections, parser->sections, parser->sectionsSz * sizeof(SectionNode));
    free(parser->sections);

    parser->sectionsSz = newSz;
    parser->sections   = newSections;
    return PARSER_OK;
}

ParserErrCode ParseTokens(Parser* parser)
{
    assert(parser != NULL);

    getNextToken(parser->toks);

    while (1)
    {
        eatBlanks(parser);
        if (currTokenType(parser) == ASM_T_EOF)
            break;

        eatToken(parser, ASM_T_SECTION); // TODO: check for duplicating sections

        eatSP(parser);

        const char* sectName = currTokenVal(parser);
        eatToken(parser, ASM_T_ID);

        ParserErrCode err = reallocSections(parser);
        if (err != PARSER_OK)
            return err;

        parser->currSection       = parser->sections + parser->sectionsSz - 1;
        parser->currSection->name = sectName;

        eatSP(parser);

        err = parseSectionNode(parser, parser->currSection);
        if (err != PARSER_OK)
            return err;
    }

    return PARSER_OK;
}

ParserErrCode ParserInit(Parser* p, Tokenizer* toks)
{
    assert(p != NULL);

    p->toks = toks;

    return PARSER_OK;
}

void parserFree(Parser* p)
{
    for (size_t i = 0; i < p->sectionsSz; i++)
    {
        SectionNode* currSect = p->sections + i;

        for (size_t j = 0; j < currSect->commandsSz; j++)
            if (currSect->commands[j].dataSz > 0)
                free(currSect->commands[j].data);

        free(currSect->commands);
    }

    while (p->userErrors != NULL)
    {
        ParserError* err = p->userErrors;
        p->userErrors    = p->userErrors->next;

        free(err);
    }

    free(p->sections);
}
