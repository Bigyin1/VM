#include <assert.h>
#include <string.h>
#include <math.h>
#include "../../vm/instructions/registers/registers.hpp"
#include "parser.hpp"
#include "labels.hpp"
#include "utils.hpp"

static DataSize evalImmMinDataSz(u_int64_t val, e_asm_token_type type)
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

static asm_ecode parseIndirectArg(parser_s *parser, commandNode *node, Argument *arg)
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
            arg->ImmDisp16 = currTokenIntNumVal(parser);
            arg->Type = ArgImmOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return E_ASM_OK;
        }

        arg->Type = ArgImmIndirect;
        return E_ASM_OK;
    }
    else if (currTokenType(parser) == ASM_T_UNSIGNED_INT)
    {
        arg->Imm = currTokenIntNumVal(parser);
        eatToken(parser, ASM_T_UNSIGNED_INT);

        if (currTokenType(parser) == ASM_T_SIGNED_INT ||
            currTokenType(parser) == ASM_T_UNSIGNED_INT) // [128+128]
        {
            arg->ImmDisp16 = currTokenIntNumVal(parser);
            arg->Type = ArgImmOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return E_ASM_OK;
        }

        arg->Type = ArgImmIndirect;
        return E_ASM_OK;
    }
    else
    {
        int regNum = FindRegByName(currTokenVal(parser));

        if (regNum < 0)
        {
            printf("asm: unknown register: %s, line: %zu\n", currTokenVal(parser), node->line);
            return E_ASM_ERR;
        }
        arg->RegNum = (uint8_t)regNum;
        eatToken(parser, ASM_T_ID);

        if (currTokenType(parser) == ASM_T_SIGNED_INT ||
            currTokenType(parser) == ASM_T_UNSIGNED_INT) // [r0+128]
        {
            arg->ImmDisp16 = currTokenIntNumVal(parser);
            arg->Type = ArgRegisterOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return E_ASM_OK;
        }

        arg->Type = ArgRegisterIndirect;
        return E_ASM_OK;
    }
}

static asm_ecode parseCommandArg(parser_s *parser, commandNode *node, Argument *arg)
{
    assert(parser != NULL);
    assert(node != NULL);

    if (currTokenType(parser) == ASM_T_NL)
        return E_ASM_OK;

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
        memcpy(&arg->Imm, &parser->toks->currToken->dblNumVal, sizeof(double));

        arg->Type = ArgImm;

        arg->_immArgSz = DataWord; // double size

        eatToken(parser, ASM_T_FLOAT);
    }
    else if (currTokenType(parser) == ASM_T_SIGNED_INT || currTokenType(parser) == ASM_T_UNSIGNED_INT)
    {
        arg->Imm = currTokenIntNumVal(parser);
        arg->Type = ArgImm;

        arg->_immArgSz = evalImmMinDataSz(arg->Imm, currTokenType(parser));

        eatToken(parser, currTokenType(parser));
    }
    else if (currTokenType(parser) == ASM_T_ID) // register name
    {
        int regNum = FindRegByName(currTokenVal(parser));
        if (regNum < 0)
        {
            printf("asm: unknown register: %s ;line: %zu\n", currTokenVal(parser), node->line);
            return E_ASM_ERR;
        }
        arg->RegNum = (uint8_t)regNum;
        arg->Type = ArgRegister;
        eatToken(parser, ASM_T_ID);
    }
    else
    {
        if (eatToken(parser, ASM_T_L_PAREN) != E_ASM_OK)
            return E_ASM_ERR;

        eatSP(parser);

        if (parseIndirectArg(parser, node, arg) != E_ASM_OK)
            return E_ASM_ERR;

        eatSP(parser);

        if (eatToken(parser, ASM_T_R_PAREN) != E_ASM_OK)
            return E_ASM_ERR;
    }

    return E_ASM_OK;
}

static asm_ecode parseInstrPostfix(parser_s *parser, commandNode *node)
{
    if (currTokenType(parser) != ASM_T_L_SIMP_PAREN)
        return E_ASM_OK;

    eatToken(parser, ASM_T_L_SIMP_PAREN);

    const char *postfix = currTokenVal(parser);
    if (eatToken(parser, ASM_T_ID) != E_ASM_OK)
        return E_ASM_ERR;

    size_t len = strlen(postfix);

    if (len > 3)
        return E_ASM_ERR;

    if (postfix[0] == 'b')
    {
        node->instr.DataSz = DataByte;
        if (len == 2 && postfix[1] == 's')
            node->instr.SignExtend = 1;
        else if (len > 1)
            return E_ASM_ERR;

        if (eatToken(parser, ASM_T_R_SIMP_PAREN) != E_ASM_OK)
            return E_ASM_ERR;

        return E_ASM_OK;
    }

    if (len < 2)
        return E_ASM_ERR;

    if (strncmp(postfix, "db", 2) == 0)
    {
        node->instr.DataSz = DataDByte;
        if (len == 3 && postfix[2] == 's')
            node->instr.SignExtend = 1;

        if (eatToken(parser, ASM_T_R_SIMP_PAREN) != E_ASM_OK)
            return E_ASM_ERR;

        return E_ASM_OK;
    }
    else if (strncmp(postfix, "hw", 2) == 0)
    {
        node->instr.DataSz = DataHalfWord;
        if (len == 3 && postfix[2] == 's')
            node->instr.SignExtend = 1;

        if (eatToken(parser, ASM_T_R_SIMP_PAREN) != E_ASM_OK)
            return E_ASM_ERR;

        return E_ASM_OK;
    }

    return E_ASM_ERR;
}

static asm_ecode createInstruction(parser_s *parser, commandNode *node)
{

    // ld(hws)

    size_t sz = 0;
    InstrDecErr err = NewInstruction(node->instrName, &node->instr, &sz);
    if (err == INSTR_UNKNOWN)
    {
        printf("asm: unknown mnemonic: %s; line: %zu\n", node->instrName, node->line);
        return E_ASM_ERR;
    }
    if (err == INSTR_WRONG_OPERANDS)
    {
        printf("asm: invalid operands for %s; line: %zu\n", node->instrName, node->line);
        return E_ASM_ERR;
    }

    parser->prog.currOffset += sz;

    return E_ASM_OK;
}

static asm_ecode parseCommandNode(parser_s *parser, commandNode *node)
{
    assert(parser != NULL);
    assert(node != NULL);

    char *name = currTokenVal(parser);

    node->offset = parser->prog.currOffset;
    if (currTokenType(parser) == ASM_T_LABEL)
    {
        node->label = name;

        if (defineNewLabel(parser, node->label, node->offset) < 0)
        {
            printf("asm: label: %s redefinded on %zu\n", node->label, parser->toks->currToken->line);
            return E_ASM_ERR;
        }
        eatToken(parser, ASM_T_LABEL);
        eatBlanks(parser);
    }

    node->line = parser->toks->currToken->line;

    name = currTokenVal(parser);

    if (eatToken(parser, ASM_T_ID) != E_ASM_OK)
        return E_ASM_ERR;

    node->instrName = name;
    if (parseInstrPostfix(parser, node) != E_ASM_OK)
        return E_ASM_ERR;

    eatSP(parser);

    if (parseCommandArg(parser, node, &node->instr.Arg1) != E_ASM_OK)
        return E_ASM_ERR;

    eatSP(parser);

    if (currTokenType(parser) == ASM_T_COMMA)
    {
        eatToken(parser, ASM_T_COMMA);
        eatSP(parser);

        if (parseCommandArg(parser, node, &node->instr.Arg2) != E_ASM_OK)
            return E_ASM_ERR;
    }

    return createInstruction(parser, node);
}

static asm_ecode parseCodeNode(parser_s *parser, codeNode *node)
{
    assert(parser != NULL);
    assert(node != NULL);

    commandNode *cmds = node->commands;
    getNextToken(parser->toks);

    eatBlanks(parser);

    while (1)
    {

        if (parseCommandNode(parser, cmds++) != E_ASM_OK)
            return E_ASM_ERR;

        if (currTokenType(parser) == ASM_T_EOF)
            break;

        eatSP(parser);

        if (eatToken(parser, ASM_T_NL) != E_ASM_OK)
            return E_ASM_ERR;

        eatBlanks(parser);

        if (currTokenType(parser) == ASM_T_EOF)
            break;
    }

    return E_ASM_OK;
}

asm_ecode parseTokens(parser_s *parser)
{
    assert(parser != NULL);

    if (parseCodeNode(parser, &parser->prog) != E_ASM_OK)
        return E_ASM_ERR;

    if (resolveImports(parser) < 0)
        return E_ASM_ERR;

    return E_ASM_OK;
}
