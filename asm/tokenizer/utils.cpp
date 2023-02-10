#include "tokenizer/tokenizer.hpp"

const char* TokenTypeVerbose(TokenType t)
{

    switch (t)
    {
        case ASM_T_ID:
            return "mnemonic or symbol name";
        case ASM_T_FLOAT:
            return "float number";
        case ASM_T_INT:
            return "signed integer";
        case ASM_T_LABEL_DEF:
            return "label defenition";
        case ASM_T_SPACE:
            return "space or tabulation";
        case ASM_T_REGISTER:
            return "register name";
        case ASM_T_SECTION:
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
        case ASM_T_NL:
            return "newline";
        case ASM_T_COMMENT:
            return "comment string";

        default:
            return "unknown";
    }
}
