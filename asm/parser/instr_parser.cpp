#include "instr_parser.hpp"

#include <assert.h>
#include <math.h>
#include <string.h>

#include "directives.hpp"
#include "encode.hpp"
#include "errors.hpp"
#include "instr_postfix_parser.hpp"
#include "registers.hpp"
#include "symbols.hpp"
#include "utils.hpp"

static ParserErrCode parseImmDisp(Parser* parser, Argument* arg, ArgType t)
{
    if (currTokenType(parser) != ASM_T_INT)
        return PARSER_INSUFF_TOKEN;

    arg->ImmDisp16 = (int16_t)currTokenNumVal(parser);
    arg->Type      = t;
    if (eatToken(parser, ASM_T_INT) != PARSER_OK)
        return PARSER_BAD_COMMAND;

    return PARSER_OK;
}

static ParserErrCode parseIndirectArg(Parser* parser, CommandNode* node, Argument* arg)
{
    assert(parser != NULL);
    assert(node != NULL);

    if (currTokenType(parser) == ASM_T_ID)
    {
        char* labelVal = currTokenVal(parser);

        addSymbolReference(parser, labelVal, node->offset + EvalInstrSymbolOffset(&node->instr));

        eatToken(parser, ASM_T_ID);

        eatSP(parser);

        arg->Type = ArgImmIndirect;
        return PARSER_OK;
    }
    else if (currTokenType(parser) == ASM_T_INT)
    {
        arg->Imm = (uint64_t)currTokenNumVal(parser);
        eatToken(parser, ASM_T_INT);

        eatSP(parser);

        arg->Type = ArgImmIndirect;
        return PARSER_OK;
    }
    else
    {

        int regNum = FindRegByName(currTokenVal(parser));

        if (regNum < 0)
        {
            if (eatToken(parser, ASM_T_REGISTER) != PARSER_OK)
                return PARSER_BAD_COMMAND;

            addUnknownRegError(parser);
            return PARSER_BAD_COMMAND;
        }

        if (eatToken(parser, ASM_T_REGISTER) != PARSER_OK)
            return PARSER_BAD_COMMAND;

        arg->RegNum = (uint8_t)regNum;

        eatSP(parser);

        ParserErrCode err = parseImmDisp(parser, arg, ArgRegisterOffsetIndirect);
        if (err != PARSER_INSUFF_TOKEN)
            return err;

        eatSP(parser);

        arg->Type = ArgRegisterIndirect;
        return PARSER_OK;
    }
}

static ParserErrCode parseCommandArg(Parser* parser, CommandNode* node, Argument* arg)
{
    assert(parser != NULL);
    assert(node != NULL);

    if (currTokenType(parser) == ASM_T_NL || currTokenType(parser) == ASM_T_EOF)
        return PARSER_OK;

    if (currTokenType(parser) == ASM_T_ID)
    {
        char* labelVal = currTokenVal(parser);

        addSymbolReference(parser, labelVal, node->offset + EvalInstrSymbolOffset(&node->instr));

        arg->Type = ArgImm;

        arg->_immArgSz = DataWord; // address size

        eatToken(parser, ASM_T_ID);
    }
    else if (currTokenType(parser) == ASM_T_FLOAT)
    {
        arg->Imm  = (uint64_t)currTokenNumVal(parser);
        arg->Type = ArgImm;

        arg->_immArgSz = DataWord; // double size

        eatToken(parser, ASM_T_FLOAT);
    }
    else if (currTokenType(parser) == ASM_T_INT)
    {
        arg->Imm  = (uint64_t)currTokenNumVal(parser);
        arg->Type = ArgImm;

        arg->_immArgSz = evalImmMinDataSz((int64_t)arg->Imm);

        eatToken(parser, currTokenType(parser));
    }
    else if (currTokenType(parser) == ASM_T_REGISTER)
    {
        int regNum = FindRegByName(currTokenVal(parser));
        if (regNum < 0)
        {
            addUnknownRegError(parser);
            return PARSER_BAD_COMMAND;
        }

        arg->RegNum = (uint8_t)regNum;
        arg->Type   = ArgRegister;
        eatToken(parser, ASM_T_REGISTER);
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

static ParserErrCode createInstruction(Parser* parser, CommandNode* node)
{
    InstrCreationErr instrErr = NewInstruction(&node->instr);
    if (instrErr == INSTR_WRONG_OPERANDS)
    {
        addCommandInvArgsError(parser, node->name, node->line);
        return PARSER_BAD_COMMAND;
    }

    parser->currSection->size += EvalInstrSize(&node->instr);

    return PARSER_OK;
}

ParserErrCode parseInstr(Parser* parser, CommandNode* node)
{
    node->instr.im = FindInsMetaByName(node->name);
    if (node->instr.im == NULL)
    {
        addUnknownCommandError(parser, node);
        return PARSER_BAD_COMMAND;
    }

    node->Type = CMD_INSTR;

    ParserErrCode err = parseInstrPostfix(parser, node);
    if (err != PARSER_OK)
        return err;

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
