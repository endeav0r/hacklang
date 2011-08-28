#ifndef parser_HEADER
#define parser_HEADER

#include "ast.h"
#include "debug.h"

#define PARSER_RULES             28
#define PARSER_RULES_MAXLEN      8
#define PARSER_LOOKAHEAD_MAXLEN  16

#define RULE_EXPR_NUM                            0
#define RULE_EXPR_EXPR_ADD_EXPR                  1
#define RULE_STMT_EXPR_ASSIGN_EXPR               2
#define RULE_EXPR_SYM                            3
#define RULE_STMT_TERM_STMT                      4
#define RULE_STMT_STMT_STMT                      5
#define RULE_EXPR_EXPR_MINUS_EXPR                6
#define RULE_EXPR_EXPR_STAR_EXPR                 7
#define RULE_EXPR_EXPR_DIV_EXPR                  8
#define RULE_COND_EXPR_LESS_EXPR                 9
#define RULE_COND_EXPR_GREATER_EXPR              10
#define RULE_STMT_END_STMT_COND_IF               11
#define RULE_COND_TERM_COND                      12
#define RULE_STMT_END_STMT_COND_WHILE            13
#define RULE_PARAM_PARENC_EXPR_PARENO_SYM        14
#define RULE_EXPR_PARENC_EXPR_PARENO             15
#define RULE_PARAM_EXPR_COMMA_EXPR               16
#define RULE_PARAMLIST_PARENC_PARAM_PARENO       17
#define RULE_PARAM_EXPR_COMMA_PARAM              18
#define RULE_STMT_END_STMT_PARAMLIST_SYM_FUNC    19
#define RULE_EXPR_PARAMLIST_SYM                  20
#define RULE_PARAMLIST_TERM_PARAMLIST            21
#define RULE_STMT_EXPR_RETURN                    22
#define RULE_STMT_TERM_EXPR                      23
#define RULE_EXPR_EXPR_MOD_EXPR                  24
#define RULE_COND_EXPR_EQUAL_EXPR                25
#define RULE_EXPR_STRING                         26
#define RULE_EXPR_PARENC_PARENO_SYM              27

struct parser_stack_s {
    struct ast_s * ast;
    struct parser_stack_s * prev;
};

struct parser_s {
    int stack_size;
    struct ast_s * ast;
    struct parser_stack_s * bottom;
    struct parser_stack_s * top;
};

struct parser_s * parser_parse (struct token_s * tokens);

struct ast_s * parser_stack_peek (struct parser_s * parser, int depth);

#endif