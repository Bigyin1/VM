#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "section_parser.hpp"
#include "command_parser.hpp"
#include "utils.hpp"

static int reallocCommands(sectionNode *sectNode)
{

    if (sectNode->commandsSz < sectNode->commandsCap)
        return 0;

    size_t newCap = sectNode->commandsCap + 36;

    commandNode *newCommands = (commandNode *)calloc(newCap, sizeof(commandNode));
    if (newCommands == NULL)
        return -1;

    memcpy(newCommands, sectNode->commands, sectNode->commandsCap * sizeof(commandNode));
    free(sectNode->commands);

    sectNode->commandsCap = newCap;
    sectNode->commands = newCommands;
    return 0;
}

asm_ecode parseSectionNode(parser_s *parser, sectionNode *sectNode)
{
    assert(parser != NULL);
    assert(sectNode != NULL);

    sectNode->commandsCap = 16;
    sectNode->commands = (commandNode *)calloc(sectNode->commandsCap, sizeof(commandNode));
    if (sectNode->commands == NULL)
        return E_ASM_ERR;

    eatBlanks(parser);

    while (1)
    {

        if (parseCommandNode(parser, sectNode->commands + sectNode->commandsSz++) != E_ASM_OK)
            return E_ASM_ERR;

        if (currTokenType(parser) == ASM_T_EOF)
            break;

        eatSP(parser);

        if (eatToken(parser, ASM_T_NL) != E_ASM_OK)
            return E_ASM_ERR;

        eatBlanks(parser);

        if (currTokenType(parser) == ASM_T_EOF)
            break;

        if (currTokenType(parser) == ASM_T_SECTION_NAME)
            break;

        if (reallocCommands(sectNode) < 0)
            return E_ASM_ERR;
    }

    return E_ASM_OK;
}
