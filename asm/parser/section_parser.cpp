#include "section_parser.hpp"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "command_parser.hpp"
#include "utils.hpp"

static ParserErrCode reallocCommands(SectionNode* sectNode)
{
    if (sectNode->commandsSz < sectNode->commandsCap)
        return PARSER_OK;

    size_t newCap = sectNode->commandsCap + 36;

    CommandNode* newCommands = (CommandNode*)calloc(newCap, sizeof(CommandNode));
    if (newCommands == NULL)
        return PARSER_SYSTEM_ERR;

    memcpy(newCommands, sectNode->commands, sectNode->commandsCap * sizeof(CommandNode));
    free(sectNode->commands);

    sectNode->commandsCap = newCap;
    sectNode->commands    = newCommands;
    return PARSER_OK;
}

ParserErrCode parseSectionNode(Parser* parser, SectionNode* sectNode)
{
    assert(parser != NULL);
    assert(sectNode != NULL);

    sectNode->commandsCap = 48;
    sectNode->commands    = (CommandNode*)calloc(sectNode->commandsCap, sizeof(CommandNode));
    if (sectNode->commands == NULL)
        return PARSER_SYSTEM_ERR;

    eatBlanks(parser);

    while (1)
    {
        ParserErrCode err = parseCommandNode(parser, sectNode->commands + sectNode->commandsSz++);
        if (err == PARSER_BAD_COMMAND)
            eatUntillNL(parser);
        else if (err != PARSER_OK)
            return err;

        if (currTokenType(parser) == ASM_T_EOF)
            break;

        eatSP(parser);

        eatToken(parser, ASM_T_NL);

        eatBlanks(parser);

        if (currTokenType(parser) == ASM_T_EOF)
            break;

        if (currTokenType(parser) == ASM_T_SECTION)
            break;

        err = reallocCommands(sectNode);
        if (err != PARSER_OK)
            return err;
    }

    return PARSER_OK;
}
