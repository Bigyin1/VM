#include <assert.h>
#include <string.h>
#include <math.h>
#include "../../vm/instructions/registers/registers.hpp"
#include "../../vm/instructions/encode.hpp"
#include "command_parser.hpp"
#include "labels.hpp"
#include "utils.hpp"
#include "directives.hpp"

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
            arg->ImmDisp16 = currTokenNumVal(parser);
            arg->Type = ArgImmOffsetIndirect;

            eatToken(parser, currTokenType(parser));
            return E_ASM_OK;
        }

        arg->Type = ArgImmIndirect;
        return E_ASM_OK;
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
            arg->ImmDisp16 = currTokenNumVal(parser);
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

    if (currTokenType(parser) == ASM_T_NL || currTokenType(parser) == ASM_T_EOF)
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

static asm_ecode parseJumpPostfix(commandNode *node, const char *postfix)
{
    if (strcmp(postfix, "eq") == 0)
    {
        node->instr.JmpType = JumpEQ;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "neq") == 0)
    {
        node->instr.JmpType = JumpNEQ;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "g") == 0)
    {
        node->instr.JmpType = JumpG;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "ge") == 0)
    {
        node->instr.JmpType = JumpGE;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "l") == 0)
    {
        node->instr.JmpType = JumpL;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "le") == 0)
    {
        node->instr.JmpType = JumpLE;
        return E_ASM_OK;
    }

    return E_ASM_ERR;
}

static asm_ecode parseInstrPostfix(parser_s *parser, commandNode *node)
{

    const char *postfix = currTokenVal(parser);
    if (eatToken(parser, ASM_T_ID) != E_ASM_OK)
        return E_ASM_ERR;

    if (strcmp(node->name, "jmp") == 0)
    {
        if (parseJumpPostfix(node, postfix) == E_ASM_OK)
            return E_ASM_OK;

        printf("asm: invalid jump postfix: %s; line: %zu\n", postfix, node->line);
        return E_ASM_ERR;
    }

    if (strcmp(postfix, "b") == 0)
    {
        node->instr.DataSz = DataByte;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "s") == 0)
    {
        node->instr.DataSz = DataWord;
        node->instr.SignExtend = 1;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "bs") == 0)
    {
        node->instr.DataSz = DataByte;
        node->instr.SignExtend = 1;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "db") == 0)
    {
        node->instr.DataSz = DataDByte;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "dbs") == 0)
    {
        node->instr.DataSz = DataDByte;
        node->instr.SignExtend = 1;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "hw") == 0)
    {
        node->instr.DataSz = DataHalfWord;
        return E_ASM_OK;
    }

    if (strcmp(postfix, "hws") == 0)
    {
        node->instr.DataSz = DataHalfWord;
        node->instr.SignExtend = 1;
        return E_ASM_OK;
    }

    printf("asm: invalid data postfix: %s; line: %zu\n", postfix, node->line);

    return E_ASM_ERR;
}

static asm_ecode createInstruction(parser_s *parser, commandNode *node)
{

    InstrEncDecErr err = NewInstruction(node->name, &node->instr);
    if (err == INSTR_UNKNOWN)
    {
        printf("asm: unknown mnemonic: %s; line: %zu\n", node->name, node->line);
        return E_ASM_ERR;
    }
    if (err == INSTR_WRONG_OPERANDS)
    {
        printf("asm: invalid operands for %s; line: %zu\n", node->name, node->line);
        return E_ASM_ERR;
    }

    parser->currSection->currOffset += EvalInstrSize(&node->instr);

    return E_ASM_OK;
}

asm_ecode parseCommandNode(parser_s *parser, commandNode *node)
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
            printf("asm: label: %s redefinded on %zu\n", node->label, parser->toks->currToken->line);
            return E_ASM_ERR;
        }
        eatToken(parser, ASM_T_LABEL);
        eatBlanks(parser);
    }

    node->line = parser->toks->currToken->line;

    node->name = currTokenVal(parser);
    if (eatToken(parser, ASM_T_ID) != E_ASM_OK)
        return E_ASM_ERR;

    e_asm_codes err = parseDataDefDirective(parser, node);
    if (err != E_ASM_INSUFF_TOKEN)
        return err;

    if (currTokenType(parser) == ASM_T_L_SIMP_PAREN)
    {
        eatToken(parser, ASM_T_L_SIMP_PAREN);
        if (parseInstrPostfix(parser, node) != E_ASM_OK)
            return E_ASM_ERR;

        if (eatToken(parser, ASM_T_R_SIMP_PAREN) != E_ASM_OK)
            return E_ASM_ERR;
    }

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
