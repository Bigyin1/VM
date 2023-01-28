#include <assert.h>
#include <string.h>
#include <math.h>
#include "registers.hpp"
#include "encode.hpp"
#include "command_parser.hpp"
#include "cmd_postfix_parser.hpp"
#include "symbols.hpp"
#include "utils.hpp"
#include "directives.hpp"
#include "errors.hpp"

static ParserErrCode parseIndirectArg(Parser *parser, commandNode *node, Argument *arg)
{
    assert(parser != NULL);
    assert(node != NULL);

    if (currTokenType(parser) == ASM_T_LABEL)
    {
        char *labelVal = currTokenVal(parser);

        addSymbolReference(parser, labelVal, node->offset + EvalInstrSymbolOffset(&node->instr));

        eatToken(parser, ASM_T_LABEL);

        if (currTokenType(parser) == ASM_T_SIGNED_INT ||
            currTokenType(parser) == ASM_T_UNSIGNED_INT) // [label+128]
        {
            arg->ImmDisp16 = currTokenNumVal(parser);
            arg->Type = ArgImmOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return PARSER_OK;
        }

        arg->Type = ArgImmIndirect;
        return PARSER_OK;
    }
    else if (currTokenType(parser) == ASM_T_UNSIGNED_INT)
    {
        arg->Imm = currTokenNumVal(parser);
        eatToken(parser, ASM_T_UNSIGNED_INT);

        if (currTokenType(parser) == ASM_T_SIGNED_INT ||
            currTokenType(parser) == ASM_T_UNSIGNED_INT) // [128+128]
        {
            arg->ImmDisp16 = currTokenNumVal(parser);
            arg->Type = ArgImmOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return PARSER_OK;
        }

        arg->Type = ArgImmIndirect;
        return PARSER_OK;
    }
    else
    {
        size_t line = currTokenLine(parser);
        size_t column = currTokenColumn(parser);
        const char *regVal = currTokenVal(parser);

        if (eatToken(parser, ASM_T_ID) != PARSER_OK)
            return PARSER_BAD_COMMAND;

        int regNum = FindRegByName(regVal);

        if (regNum < 0)
        {
            ParserError *err = addNewParserError(parser, PARSER_UNKNOWN_REGISTER);

            err->token = regVal;
            err->line = line;
            err->column = column;
        }
        arg->RegNum = (uint8_t)regNum;

        if (currTokenType(parser) == ASM_T_SIGNED_INT ||
            currTokenType(parser) == ASM_T_UNSIGNED_INT) // [r0+128]
        {
            arg->ImmDisp16 = currTokenNumVal(parser);
            arg->Type = ArgRegisterOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return PARSER_OK;
        }

        arg->Type = ArgRegisterIndirect;
        return PARSER_OK;
    }
}

static ParserErrCode parseCommandArg(Parser *parser, commandNode *node, Argument *arg)
{
    assert(parser != NULL);
    assert(node != NULL);

    if (currTokenType(parser) == ASM_T_NL || currTokenType(parser) == ASM_T_EOF)
        return PARSER_OK;

    if (currTokenType(parser) == ASM_T_LABEL)
    {
        char *labelVal = currTokenVal(parser);

        addSymbolReference(parser, labelVal, node->offset + EvalInstrSymbolOffset(&node->instr));

        arg->Type = ArgImm;

        arg->_immArgSz = DataWord; // address size

        eatToken(parser, ASM_T_LABEL);
    }
    else if (currTokenType(parser) == ASM_T_FLOAT)
    {
        arg->Imm = currTokenNumVal(parser);
        arg->Type = ArgImm;

        arg->_immArgSz = DataWord; // double size

        eatToken(parser, ASM_T_FLOAT);
    }
    else if (currTokenType(parser) == ASM_T_SIGNED_INT || currTokenType(parser) == ASM_T_UNSIGNED_INT)
    {
        arg->Imm = currTokenNumVal(parser);
        arg->Type = ArgImm;

        arg->_immArgSz = evalImmMinDataSz(arg->Imm, currTokenType(parser));

        eatToken(parser, currTokenType(parser));
    }
    else if (currTokenType(parser) == ASM_T_ID) // register name
    {
        int regNum = FindRegByName(currTokenVal(parser));
        if (regNum < 0)
        {
            ParserError *err = addNewParserError(parser, PARSER_UNKNOWN_REGISTER);

            err->token = currTokenVal(parser);
            err->line = currTokenLine(parser);
            err->column = currTokenColumn(parser);
        }

        arg->RegNum = (uint8_t)regNum;
        arg->Type = ArgRegister;
        eatToken(parser, ASM_T_ID);
    }
    else
    {
        if (eatToken(parser, ASM_T_L_PAREN) != PARSER_OK)
            return PARSER_BAD_COMMAND;

        eatSP(parser);

        ParserErrCode err = parseIndirectArg(parser, node, arg);
        if (err != PARSER_OK)
            return err;

        eatSP(parser);

        if (eatToken(parser, ASM_T_R_PAREN) != PARSER_OK)
            return PARSER_BAD_COMMAND;
    }

    return PARSER_OK;
}

static ParserErrCode createInstruction(Parser *parser, commandNode *node)
{

    InstrCreationErr instrErr = NewInstruction(node->name, &node->instr);
    if (instrErr == INSTR_WRONG_OPERANDS)
    {
        ParserError *err = addNewParserError(parser, PARSER_COMMAND_INV_ARGS);

        err->token = node->name;
        err->line = node->line;
        return PARSER_BAD_COMMAND;
    }

    parser->currSection->currOffset += EvalInstrSize(&node->instr);

    return PARSER_OK;
}

ParserErrCode parseCommandNode(Parser *parser, commandNode *node)
{
    assert(parser != NULL);
    assert(node != NULL);

    char *name = currTokenVal(parser);
    node->offset = parser->currSection->currOffset;

    if (currTokenType(parser) == ASM_T_LABEL)
    {
        node->label = name;

        if (defineNewSymbol(parser, node->label, node->offset) == PARSER_LABEL_REDEF)
        {
            ParserError *err = addNewParserError(parser, PARSER_LABEL_REDEF);

            err->token = node->label;
            err->line = currTokenLine(parser);
            err->column = currTokenColumn(parser);
        }
        eatToken(parser, ASM_T_LABEL);
        eatBlanks(parser);
    }

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

    node->Type = CMD_INSTR;
    node->instr.im = FindInsMetaByName(node->name);
    if (node->instr.im == NULL)
    {
        ParserError *err = addNewParserError(parser, PARSER_UNKNOWN_COMMAND);

        err->token = node->name;
        err->line = node->line;
        return PARSER_BAD_COMMAND;
    }

    if (currTokenType(parser) == ASM_T_L_SIMP_PAREN)
    {
        eatToken(parser, ASM_T_L_SIMP_PAREN);

        err = parseInstrPostfix(parser, node);
        if (err != PARSER_OK)
            return err;

        if (eatToken(parser, ASM_T_R_SIMP_PAREN) != PARSER_OK)
            return PARSER_BAD_COMMAND;
    }

    eatSP(parser);

    err = parseCommandArg(parser, node, &node->instr.Arg1);
    if (err != PARSER_OK)
        return err;

    eatSP(parser);

    if (currTokenType(parser) == ASM_T_COMMA)
    {
        eatToken(parser, ASM_T_COMMA);
        eatSP(parser);

        err = parseCommandArg(parser, node, &node->instr.Arg2);
        if (err != PARSER_OK)
            return err;
    }

    return createInstruction(parser, node);
}
