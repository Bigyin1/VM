#include <assert.h>
#include <string.h>
#include "command_parser.hpp"
#include "instr_parser.hpp"
#include "utils.hpp"
#include "symbols.hpp"
#include "directives.hpp"
#include "errors.hpp"

static ParserErrCode parseCmdLabel(Parser *parser, commandNode *node)
{

    if (currTokenType(parser) != ASM_T_LABEL)
        return PARSER_OK;

    node->label = currTokenVal(parser);

    if (defineNewSymbol(parser, node->label, node->offset) == PARSER_LABEL_REDEF)
    {
        ParserError *err = addNewParserError(parser, PARSER_LABEL_REDEF);

        err->token = node->label;
        err->line = currTokenLine(parser);
        err->column = currTokenColumn(parser);
    }
    eatToken(parser, ASM_T_LABEL);
    eatBlanks(parser);

    return PARSER_OK;
}

ParserErrCode parseCommandNode(Parser *parser, commandNode *node)
{
    assert(parser != NULL);
    assert(node != NULL);

    node->offset = parser->currSection->currOffset;

    parseCmdLabel(parser, node);

    node->line = parser->toks->currToken->line;
    node->name = currTokenVal(parser);
    if (eatToken(parser, ASM_T_ID) != PARSER_OK)
        return PARSER_BAD_COMMAND;

    ParserErrCode err = parseControlDirective(parser, node);
    if (err != PARSER_INSUFF_TOKEN)
        return err;

    err = parseDataDefDirective(parser, node);
    if (err != PARSER_INSUFF_TOKEN)
        return err;

    return parseInstr(parser, node);
}
