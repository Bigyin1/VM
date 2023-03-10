#include "directives.hpp"

#include <stdlib.h>
#include <string.h>

#include "errors.hpp"
#include "symbols.hpp"
#include "utils.hpp"

typedef struct dataDirective
{
    const char* name;
    DataSize    sz;

} dataDirective;

static const dataDirective dataDirectives[] = {

    {
        .name = "db",
        .sz   = DataByte,
    },
    {
        .name = "ddb",
        .sz   = DataDByte,
    },
    {
        .name = "dhw",
        .sz   = DataHalfWord,
    },
    {
        .name = "dw",
        .sz   = DataWord,
    },

};

static ParserErrCode getDataDefDirectiveArgsCount(Parser* parser, size_t* count)
{

    saveCurrToken(parser->toks);

    eatBlanks(parser);

    while (currTokenType(parser) == ASM_T_FLOAT || currTokenType(parser) == ASM_T_INT ||
           currTokenType(parser) == ASM_T_LABEL_DEF)
    {
        (*count)++;

        eatToken(parser, currTokenType(parser));
        eatBlanks(parser);

        if (currTokenType(parser) != ASM_T_COMMA)
            break;

        eatToken(parser, ASM_T_COMMA);
        eatBlanks(parser);
    }

    if (*count == 0)
    {
        addCommandInvArgsError(parser, currTokenVal(parser), currTokenLine(parser));
        return PARSER_BAD_COMMAND;
    }
    return PARSER_OK;
}

static ParserErrCode parseDataDefDirectiveArgs(Parser* parser, CommandNode* node, size_t sz)
{
    size_t        argsCount = 0;
    ParserErrCode err       = getDataDefDirectiveArgsCount(parser, &argsCount);
    if (err != PARSER_OK)
        return err;

    node->data   = (char*)calloc(argsCount, sz);
    node->dataSz = argsCount * sz;
    restoreSavedToken(parser->toks);

    eatBlanks(parser);

    size_t dataIdx = 0;

    while (currTokenType(parser) == ASM_T_FLOAT || currTokenType(parser) == ASM_T_INT ||
           currTokenType(parser) == ASM_T_LABEL_DEF)
    {
        argsCount--;

        if (currTokenType(parser) != ASM_T_LABEL_DEF)
            memcpy(node->data + dataIdx, &currTokenNumVal(parser), sz);
        else
            addSymbolReference(parser, currTokenVal(parser), node->offset + dataIdx);

        dataIdx += sz;

        eatToken(parser, currTokenType(parser));
        eatSP(parser);

        if (argsCount == 0)
            break;

        if (currTokenType(parser) != ASM_T_COMMA)
            break;

        eatToken(parser, ASM_T_COMMA);
        eatBlanks(parser);
    }

    node->Type = CMD_DATA_DEF;

    parser->currSection->size += node->dataSz;

    return PARSER_OK;
}

ParserErrCode parseDataDefDirective(Parser* parser, CommandNode* node)
{

    for (size_t i = 0; i < sizeof(dataDirectives) / sizeof(dataDirective); i++)
    {
        if (strcmp(dataDirectives[i].name, node->name) == 0)
            return parseDataDefDirectiveArgs(parser, node, DataSzToBytesSz(dataDirectives[i].sz));
    }

    return PARSER_INSUFF_TOKEN;
}

ParserErrCode parseControlDirective(Parser* parser, CommandNode* node)
{

    if (strcmp("equ", node->name) != 0)
        return PARSER_INSUFF_TOKEN;

    eatSP(parser);

    const char* symb = currTokenVal(parser);
    if (eatToken(parser, ASM_T_LABEL_DEF) != PARSER_OK)
        return PARSER_BAD_COMMAND;

    eatSP(parser);

    if (currTokenType(parser) != ASM_T_FLOAT && currTokenType(parser) != ASM_T_INT)
    {
        eatToken(parser, ASM_T_INT);
        return PARSER_BAD_COMMAND;
    }

    if (defineNewAbsSymbol(parser, symb, (uint64_t)currTokenNumVal(parser)) == PARSER_LABEL_REDEF)
    {
        addLabelRedefError(parser, symb);
        return PARSER_BAD_COMMAND;
    }

    eatToken(parser, currTokenType(parser));

    node->Type = CMD_CONTROL;

    return PARSER_OK;
}
