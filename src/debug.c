#include "debug.h"


char * tok_debug_string (int type)
{
    switch (type) {
    case TOK_NUM :
        return "TOK_NUM";
    case TOK_SYM :
        return "TOK_SYM";
    case TOK_ADD :
        return "TOK_ADD";
    case TOK_TERM :
        return "TOK_TERM";
    case TOK_ASSIGN :
        return "TOK_ASSIGN";
    case TOK_STAR :
        return "TOK_STAR";
    case TOK_MINUS :
        return "TOK_MINUS";
    case TOK_DIV :
        return "TOK_DIV";
    case TOK_IF :
        return "TOK_IF";
    case TOK_END :
        return "TOK_END";
    case TOK_LESS :
        return "TOK_LESS";
    case TOK_GREATER :
        return "TOK_GREATER";
    case TOK_WHILE :
        return "TOK_WHILE";
    case TOK_EQUAL :
        return "TOK_EQUAL";
    case TOK_PAREN_O :
        return "TOK_PAREN_O";
    case TOK_PAREN_C :
        return "TOK_PAREN_C";
    case TOK_COMMA :
        return "TOK_COMMA";
    case TOK_BRACK_O :
        return "TOK_BRACK_O";
    case TOK_BRACK_C :
        return "TOK_BRACK_C";
    case TOK_FUNC :
        return "TOK_FUNC";
    case TOK_RETURN :
        return "TOK_RETURN";
    case TOK_EXPR :
        return "TOK_EXPR";
    case TOK_MOD :
        return "TOK_MOD";
    case TOK_STRING :
        return "TOK_STRING";
    case TOK_STMT :
        return "TOK_STMT";
    case TOK_COND :
        return "TOK_COND";
    case TOK_BRANCH :
        return "TOK_BRANCH";
    case TOK_PARAM :
        return "TOK_PARAM";
    case TOK_FUNCDEC :
        return "TOK_FUNCDEC";
    case TOK_LOOP :
        return "TOK_LOOP";
    case TOK_PARAMLIST :
        return "TOK_PARAMLIST";
    }
    return "TOK_UNKNOWN";
}