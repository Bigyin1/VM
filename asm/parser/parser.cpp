#include <assert.h>
#include <string.h>
#include <math.h>
#include "parser.hpp"
#include "section_parser.hpp"
#include "utils.hpp"
#include "labels.hpp"

static int reallocSections(parser_s *parser)
{
    if (parser->sectionsSz == 0)
    {
        parser->sections = (sectionNode *)calloc(++parser->sectionsSz, sizeof(sectionNode));
        return 0;
    }

    size_t newSz = parser->sectionsSz + 1;
    sectionNode *newSections = (sectionNode *)calloc(newSz, sizeof(sectionNode));

    if (newSections == NULL)
        return -1;

    memcpy(newSections, parser->sections, parser->sectionsSz * sizeof(sectionNode));
    free(parser->sections);

    parser->sectionsSz = newSz;
    parser->sections = newSections;
    return 0;
}

asm_ecode parseTokens(parser_s *parser)
{
    assert(parser != NULL);

    getNextToken(parser->toks);

    while (1)
    {
        eatBlanks(parser);
        if (currTokenType(parser) == ASM_T_EOF)
            break;

        const char *sectName = currTokenVal(parser);

        if (eatToken(parser, ASM_T_SECTION_NAME) != E_ASM_OK)
            return E_ASM_ERR;

        if (reallocSections(parser) < 0)
            return E_ASM_ERR;

        parser->currSection = parser->sections + parser->sectionsSz - 1;
        parser->currSection->name = sectName;

        eatSP(parser);
        uint64_t sectAddr = currTokenNumVal(parser);
        if (eatToken(parser, ASM_T_UNSIGNED_INT) != E_ASM_OK)
            return E_ASM_ERR;

        parser->currSection->addr = sectAddr;

        if (parseSectionNode(parser, parser->currSection) != E_ASM_OK)
            return E_ASM_ERR;
    }

    if (resolveImports(parser) < 0)
        return E_ASM_ERR;

    return E_ASM_OK;
}

asm_ecode initParser(parser_s *p, tokenizer_s *toks)
{
    assert(p != NULL);

    p->toks = toks;

    return E_ASM_OK;
}

void parserFree(parser_s *p)
{
    for (size_t i = 0; i < p->sectionsSz; i++)
    {
        sectionNode *currSect = p->sections + i;

        free(currSect->commands);
    }

    free(p->sections);
}
