#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "section_parser.hpp"
#include "command_parser.hpp"
#include "utils.hpp"

static ParserErrCode reallocCommands(sectionNode *sectNode)
{

    if (sectNode->commandsSz < sectNode->commandsCap)
        return PARSER_OK;

    size_t newCap = sectNode->commandsCap + 36;

    commandNode *newCommands = (commandNode *)calloc(newCap, sizeof(commandNode));
    if (newCommands == NULL)
        return PARSER_SYSTEM_ERR;

    memcpy(newCommands, sectNode->commands, sectNode->commandsCap * sizeof(commandNode));
    free(sectNode->commands);

    sectNode->commandsCap = newCap;
    sectNode->commands = newCommands;
    return PARSER_OK;
}

ParserErrCode parseSectionNode(Parser *parser, sectionNode *sectNode)
{
    assert(parser != NULL);
    assert(sectNode != NULL);

    sectNode->commandsCap = 48;
    sectNode->commands = (commandNode *)calloc(sectNode->commandsCap, sizeof(commandNode));
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

        if (currTokenType(parser) == ASM_T_SECTION_NAME)
            break;

        err = reallocCommands(sectNode);
        if (err != PARSER_OK)
            return err;
    }

    return PARSER_OK;
}
