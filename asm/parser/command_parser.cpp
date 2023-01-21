#include <assert.h>
#include <string.h>
#include <math.h>
#include "registers.hpp"
#include "encode.hpp"
#include "command_parser.hpp"
#include "labels.hpp"
#include "utils.hpp"
#include "directives.hpp"
#include "errors.hpp"

static DataSize evalImmMinDataSz(u_int64_t val, TokenType type)
{
    if (type == ASM_T_UNSIGNED_INT)
    {
        uint64_t uVal = val;
        if (uVal <= UINT8_MAX)
            return DataByte;
        if (uVal <= UINT16_MAX)
            return DataDByte;
        if (uVal <= UINT32_MAX)
            return DataHalfWord;
        return DataWord;
    }
    int64_t sVal = val;

    if (sVal <= INT8_MAX && sVal >= INT8_MIN)
        return DataByte;
    if (sVal <= INT16_MAX && sVal >= INT16_MIN)
        return DataDByte;
    if (sVal <= INT32_MAX && sVal >= INT32_MIN)
        return DataHalfWord;
    return DataWord;
}

static ParserErrCode parseIndirectArg(Parser *parser, commandNode *node, Argument *arg)
{
    assert(parser != NULL);
    assert(node != NULL);

    if (currTokenType(parser) == ASM_T_LABEL)
    {
        char *labelVal = currTokenVal(parser);
        addLabelImport(parser, labelVal, &arg->Imm);
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
        addLabelImport(parser, labelVal, &arg->Imm);
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
            getNextToken(parser->toks);

        eatSP(parser);

        ParserErrCode err = parseIndirectArg(parser, node, arg);
        if (err != PARSER_OK)
            return err;

        eatSP(parser);

        if (eatToken(parser, ASM_T_R_PAREN) != PARSER_OK)
            getNextToken(parser->toks);
    }

    return PARSER_OK;
}

static ParserErrCode parseJumpPostfix(commandNode *node, const char *postfix)
{
    if (strcmp(postfix, "eq") == 0)
    {
        node->instr.JmpType = JumpEQ;
        return PARSER_OK;
    }

    if (strcmp(postfix, "neq") == 0)
    {
        node->instr.JmpType = JumpNEQ;
        return PARSER_OK;
    }

    if (strcmp(postfix, "g") == 0)
    {
        node->instr.JmpType = JumpG;
        return PARSER_OK;
    }

    if (strcmp(postfix, "ge") == 0)
    {
        node->instr.JmpType = JumpGE;
        return PARSER_OK;
    }

    if (strcmp(postfix, "l") == 0)
    {
        node->instr.JmpType = JumpL;
        return PARSER_OK;
    }

    if (strcmp(postfix, "le") == 0)
    {
        node->instr.JmpType = JumpLE;
        return PARSER_OK;
    }

    return PARSER_INSUFF_TOKEN;
}

static ParserErrCode parseInstrPostfix(Parser *parser, commandNode *node)
{

    const char *postfix = currTokenVal(parser);
    size_t line = currTokenLine(parser);
    size_t column = currTokenColumn(parser);

    if (eatToken(parser, ASM_T_ID) != PARSER_OK)
        return PARSER_BAD_COMMAND;

    if (strcmp(node->name, "jmp") == 0)
    {
        if (parseJumpPostfix(node, postfix) == PARSER_OK)
            return PARSER_OK;

        ParserError *err = addNewParserError(parser, PARSER_BAD_CMD_POSTFIX);

        err->token = postfix;
        err->line = line;
        err->column = column;

        return PARSER_OK;
    }

    if (strcmp(postfix, "b") == 0)
    {
        node->instr.DataSz = DataByte;
        return PARSER_OK;
    }

    if (strcmp(postfix, "s") == 0)
    {
        node->instr.DataSz = DataWord;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    if (strcmp(postfix, "bs") == 0)
    {
        node->instr.DataSz = DataByte;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    if (strcmp(postfix, "db") == 0)
    {
        node->instr.DataSz = DataDByte;
        return PARSER_OK;
    }

    if (strcmp(postfix, "dbs") == 0)
    {
        node->instr.DataSz = DataDByte;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    if (strcmp(postfix, "hw") == 0)
    {
        node->instr.DataSz = DataHalfWord;
        return PARSER_OK;
    }

    if (strcmp(postfix, "hws") == 0)
    {
        node->instr.DataSz = DataHalfWord;
        node->instr.SignExtend = 1;
        return PARSER_OK;
    }

    ParserError *err = addNewParserError(parser, PARSER_BAD_CMD_POSTFIX);

    err->token = postfix;
    err->line = line;
    err->column = column;

    return PARSER_OK;
}

static ParserErrCode createInstruction(Parser *parser, commandNode *node)
{

    InstrEncDecErr err = NewInstruction(node->name, &node->instr);
    if (err == INSTR_UNKNOWN)
    {

        ParserError *err = addNewParserError(parser, PARSER_UNKNOWN_COMMAND);

        err->token = node->name;
        err->line = node->line;
        return PARSER_OK;
    }
    if (err == INSTR_WRONG_OPERANDS)
    {
        ParserError *err = addNewParserError(parser, PARSER_COMMAND_INV_ARGS);

        err->token = node->name;
        err->line = node->line;
        return PARSER_OK;
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

        if (defineNewLabel(parser, node->label, parser->currSection->addr + node->offset) < 0)
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

    ParserErrCode err = parseDataDefDirective(parser, node);
    if (err != PARSER_INSUFF_TOKEN)
        return err;

    if (currTokenType(parser) == ASM_T_L_SIMP_PAREN)
    {
        eatToken(parser, ASM_T_L_SIMP_PAREN);

        err = parseInstrPostfix(parser, node);
        if (err != PARSER_OK)
            return err;

        if (eatToken(parser, ASM_T_R_SIMP_PAREN) != PARSER_OK)
            getNextToken(parser->toks);
    }

    eatSP(parser);

    parseCommandArg(parser, node, &node->instr.Arg1);

    eatSP(parser);

    if (currTokenType(parser) == ASM_T_COMMA)
    {
        eatToken(parser, ASM_T_COMMA);
        eatSP(parser);

        parseCommandArg(parser, node, &node->instr.Arg2);
    }

    return createInstruction(parser, node);
}
