#include <assert.h>
#include <string.h>
#include <math.h>
#include "parser/parser.hpp"
#include "section_parser.hpp"
#include "utils.hpp"
#include "labels.hpp"

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

ParserErrCode parseTokens(Parser *parser)
{
    assert(parser != NULL);

    getNextToken(parser->toks);

    while (1)
    {
        eatBlanks(parser);
        if (currTokenType(parser) == ASM_T_EOF)
            break;

        const char *sectName = currTokenVal(parser);

        eatToken(parser, ASM_T_SECTION_NAME);

        ParserErrCode err = reallocSections(parser);
        if (err != PARSER_OK)
            return err;

        parser->currSection = parser->sections + parser->sectionsSz - 1;
        parser->currSection->name = sectName;

        eatSP(parser);

        uint64_t sectAddr = currTokenNumVal(parser);
        eatToken(parser, ASM_T_UNSIGNED_INT);

        parser->currSection->addr = sectAddr;

        err = parseSectionNode(parser, parser->currSection);
        if (err != PARSER_OK)
            return err;
    }

    return resolveImports(parser);
}

int initParser(Parser *p, Tokenizer *toks)
{
    assert(p != NULL);

    p->toks = toks;

    return 0;
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

    while (p->err != NULL)
    {
        ParserError *err = p->err;
        p->err = p->err->next;

        free(err);
    }

    free(p->sections);
}
