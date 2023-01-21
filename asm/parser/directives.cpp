#include <string.h>
#include <stdlib.h>
#include "directives.hpp"
#include "utils.hpp"
#include "labels.hpp"
#include "errors.hpp"

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

static ParserErrCode getDataDefDirectiveArgsCount(Parser *parser, commandNode *node, size_t *sz)
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
        ParserError *err = addNewParserError(parser, PARSER_BAD_CMD_POSTFIX);

        err->token = currTokenVal(parser);
        err->line = currTokenLine(parser);
        err->column = currTokenColumn(parser);

        return PARSER_COMMAND_INV_ARGS;
    }
    return PARSER_OK;
}

static ParserErrCode parseDataDefDirectiveArgs(Parser *parser, commandNode *node, size_t sz)
{
    size_t count = 0;
    ParserErrCode err = getDataDefDirectiveArgsCount(parser, node, &count);
    if (err != PARSER_OK)
        return err;

    node->data = (char *)calloc(count, sz);
    node->dataSz = count * sz;
    restoreSavedToken(parser->toks);

    eatBlanks(parser);

    size_t dataIdx = 0;

    while (currTokenType(parser) == ASM_T_FLOAT ||
           currTokenType(parser) == ASM_T_UNSIGNED_INT ||
           currTokenType(parser) == ASM_T_SIGNED_INT ||
           currTokenType(parser) == ASM_T_LABEL)
    {
        count--;

        if (currTokenType(parser) != ASM_T_LABEL)
            memcpy(node->data + dataIdx, &currTokenNumVal(parser), sz);
        else
            addLabelImport(parser, currTokenVal(parser), (uint64_t *)(node->data + dataIdx));

        dataIdx += sz;

        eatToken(parser, currTokenType(parser));
        eatSP(parser);

        if (count == 0)
            break;

        if (currTokenType(parser) != ASM_T_COMMA)
            break;

        eatToken(parser, ASM_T_COMMA);
        eatBlanks(parser);
    }

    return PARSER_OK;
}

ParserErrCode parseDataDefDirective(Parser *parser, commandNode *node)
{

    for (size_t i = 0; i < sizeof(dataDirectives) / sizeof(dataDirective); i++)
    {
        if (strcmp(dataDirectives[i].name, node->name) == 0)
            return parseDataDefDirectiveArgs(parser, node, DataSzToBytesSz(dataDirectives[i].sz));
    }

    return PARSER_INSUFF_TOKEN;
}
