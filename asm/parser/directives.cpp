#include <string.h>
#include <stdlib.h>
#include "directives.hpp"
#include "utils.hpp"

typedef struct dataDirective
{
    const char *name;
    DataSize sz;

} dataDirective;

static const dataDirective dataDirectives[] = {

    {
        .name = "db",
        .sz = DataByte,
    },
    {
        .name = "ddb",
        .sz = DataDByte,
    },
    {
        .name = "dhw",
        .sz = DataHalfWord,
    },
    {
        .name = "dw",
        .sz = DataWord,
    },

};

static asm_ecode getDataDefDirectiveArgsCount(parser_s *parser, commandNode *node, size_t *sz)
{

    saveCurrToken(parser->toks);

    eatBlanks(parser);

    while (currTokenType(parser) == ASM_T_FLOAT ||
           currTokenType(parser) == ASM_T_UNSIGNED_INT ||
           currTokenType(parser) == ASM_T_SIGNED_INT ||
           currTokenType(parser) == ASM_T_LABEL)
    {
        (*sz)++;

        eatToken(parser, currTokenType(parser));
        eatBlanks(parser);

        if (currTokenType(parser) != ASM_T_COMMA)
            break;

        eatToken(parser, ASM_T_COMMA);
        eatBlanks(parser);
    }

    if (*sz == 0)
    {
        printf("asm: bad data defenition directive argument; line: %zu\n", node->line);
        return E_ASM_ERR;
    }
    return E_ASM_OK;
}

static asm_ecode parseDataDefDirectiveArgs(parser_s *parser, commandNode *node, DataSize sz)
{
    size_t count = 0;

    if (getDataDefDirectiveArgsCount(parser, node, &count) == E_ASM_ERR)
        return E_ASM_ERR;

    node->data = (char *)calloc(count, DataSzToBytesSz(sz));
    restoreSavedToken(parser->toks);

    eatBlanks(parser);

    size_t dataIdx = 0;

    while (currTokenType(parser) == ASM_T_FLOAT ||
           currTokenType(parser) == ASM_T_UNSIGNED_INT ||
           currTokenType(parser) == ASM_T_SIGNED_INT ||
           currTokenType(parser) == ASM_T_LABEL)
    {

        if (currTokenType(parser) == ASM_T_FLOAT)
            memcpy(node->data + dataIdx, &currTokenDblNumVal(parser), DataSzToBytesSz(sz));
        else
            memcpy(node->data + dataIdx, &currTokenIntNumVal(parser), DataSzToBytesSz(sz));

        dataIdx += DataSzToBytesSz(sz);

        eatToken(parser, currTokenType(parser));
        eatBlanks(parser);

        if (currTokenType(parser) != ASM_T_COMMA)
            break;

        eatToken(parser, ASM_T_COMMA);
        eatBlanks(parser);
    }

    return E_ASM_OK;
}

asm_ecode parseDataDefDirective(parser_s *parser, commandNode *node)
{

    for (size_t i = 0; i < sizeof(dataDirectives) / sizeof(dataDirective); i++)
    {
        if (strcmp(dataDirectives[i].name, node->name) == 0)
            return parseDataDefDirectiveArgs(parser, node, dataDirectives[i].sz);
    }

    return E_ASM_INSUFF_TOKEN;
}
