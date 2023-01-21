#include "tokenizer/tokenizer.hpp"

const char *tokenTypeVerbose(TokenType t)
{

    switch (t)
    {
    case ASM_T_ID:
        return "name";
    case ASM_T_FLOAT:
        return "float number";
    case ASM_T_SIGNED_INT:
        return "signed integer";
    case ASM_T_UNSIGNED_INT:
        return "unsigned integer";
    case ASM_T_LABEL:
        return "label";
    case ASM_T_SPACE:
        return "space or tabulation";
    case ASM_T_SECTION_NAME:
        return "section name";
    case ASM_T_COMMA:
        return "comma";
    case ASM_T_PLUS:
        return "plus sign";
    case ASM_T_MINUS:
        return "minus sign";
    case ASM_T_L_PAREN:
        return "[";
    case ASM_T_R_PAREN:
        return "]";
    case ASM_T_L_SIMP_PAREN:
        return "(";
    case ASM_T_R_SIMP_PAREN:
        return ")";
    case ASM_T_NL:
        return "newline";
    case ASM_T_COMMENT:
        return "comment string";

    default:
        return "unknown";
    }
}
