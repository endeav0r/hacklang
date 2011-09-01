#include "parser.h"

// reverse order
int MATCH [PARSER_RULES][PARSER_RULES_MAXLEN] = {
/* 00 */ {TOK_NUM,     -1},
/* 01 */ {TOK_EXPR,    TOK_ADD,       TOK_EXPR, -1},
/* 02 */ {TOK_EXPR,    TOK_ASSIGN,    TOK_EXPR, -1},
/* 03 */ {TOK_SYM,     -1},
/* 04 */ {TOK_TERM,    TOK_STMT,      -1},
/* 05 */ {TOK_STMT,    TOK_STMT,      -1},
/* 06 */ {TOK_EXPR,    TOK_MINUS,     TOK_EXPR, -1},
/* 07 */ {TOK_EXPR,    TOK_STAR,      TOK_EXPR, -1},
/* 08 */ {TOK_EXPR,    TOK_DIV,       TOK_EXPR, -1},
/* 09 */ {TOK_EXPR,    TOK_LESS,      TOK_EXPR, -1},
/* 10 */ {TOK_EXPR,    TOK_GREATER,   TOK_EXPR, -1},
/* 11 */ {TOK_END,     TOK_STMT,      TOK_EXPR, TOK_IF, -1},
/* 12 */ {TOK_TERM,    TOK_EXPR,      TOK_IF, -1},
/* 13 */ {TOK_END,     TOK_STMT,      TOK_EXPR, TOK_WHILE, -1},
/* 14 */ {TOK_PAREN_C, TOK_EXPR,      TOK_PAREN_O, TOK_SYM, -1},
/* 15 */ {TOK_PAREN_C, TOK_EXPR,      TOK_PAREN_O, -1},
/* 16 */ {TOK_EXPR,    TOK_COMMA,     TOK_EXPR, -1},
/* 17 */ {TOK_PAREN_C, TOK_PARAM,     TOK_PAREN_O, -1},
/* 18 */ {TOK_EXPR,    TOK_COMMA,     TOK_PARAM, -1},
/* 19 */ {TOK_END,     TOK_STMT,      TOK_PARAMLIST, TOK_SYM, TOK_FUNC, -1},
/* 20 */ {TOK_PARAMLIST, TOK_SYM,     -1},
/* 21 */ {TOK_TERM,    TOK_PARAMLIST, -1},
/* 22 */ {TOK_EXPR,    TOK_RETURN,    -1},
/* 23 */ {TOK_TERM,    TOK_EXPR,      TOK_WHILE, -1},
/* 24 */ {TOK_EXPR,    TOK_MOD,       TOK_EXPR, -1},
/* 25 */ {TOK_EXPR,    TOK_EQUAL,     TOK_EXPR, -1},
/* 26 */ {TOK_STRING,  -1},
/* 27 */ {TOK_PAREN_C, TOK_PAREN_O,   TOK_SYM, -1},
/* 28 */ {TOK_TRUE,    -1},
/* 29 */ {TOK_FALSE,   -1},
/* 30 */ {TOK_TERM,    TOK_EXPR,      -1},
/* 31 */ {TOK_END,     TOK_STMT,      TOK_ELSE, -1},
/* 32 */ {TOK_END,     TOK_ELSEBLOCK, TOK_STMT, TOK_EXPR, TOK_IF, -1},
/* 33 */ {TOK_TERM,    TOK_ELSE,      -1}
};

int LOOKAHEAD [PARSER_RULES][PARSER_LOOKAHEAD_MAXLEN] = {
/* 00 */ {-1},
/* 01 */ {TOK_DIV, TOK_STAR, TOK_PAREN_O, -1},
/* 02 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 03 */ {TOK_PAREN_O, -1},
/* 04 */ {-1},
/* 05 */ {-1},
/* 06 */ {TOK_PAREN_O, TOK_DIV, TOK_STAR, -1},
/* 07 */ {TOK_PAREN_O, -1},
/* 08 */ {TOK_PAREN_O, -1},
/* 09 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 10 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 11 */ {-1},
/* 12 */ {-1},
/* 13 */ {-1},
/* 14 */ {-1},
/* 15 */ {-1},
/* 16 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 17 */ {-1},
/* 18 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 19 */ {-1},
/* 20 */ {-1},
/* 21 */ {-1},
/* 22 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 23 */ {-1},
/* 24 */ {TOK_PAREN_O, -1},
/* 25 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 26 */ {-1},
/* 27 */ {-1},
/* 28 */ {-1},
/* 29 */ {-1},
/* 30 */ {-1},
/* 31 */ {-1},
/* 32 */ {-1},
/* 33 */ {-1}
};


void parser_stack_push (struct parser_s * parser, struct ast_s * ast)
{
    struct parser_stack_s * stack;
    
    stack = (struct parser_stack_s *) malloc(sizeof(struct parser_stack_s));
    stack->ast = ast;
    if (parser->bottom == NULL) {
        stack->prev = NULL;
        parser->bottom = stack;
        parser->top = stack;
    }
    else {
        stack->prev = parser->top;
        parser->top = stack;
    }
    parser->stack_size++;
}


void parser_stack_pop (struct parser_s * parser, int depth)
{
    struct parser_stack_s * stack;
    
    while (depth--) {
        stack = parser->top;
        parser->top = stack->prev;
        if (parser->top == NULL)
            parser->bottom = NULL;
        free(stack);
        parser->stack_size--;
    }
}


struct ast_s * parser_stack_peek (struct parser_s * parser, int depth)
{
    struct parser_stack_s * stack;
    
    stack = parser->top;
    while ((stack != NULL) && (depth > 0)) {
        stack = stack->prev;
        depth--;
    }
    
    if (stack == NULL)
        return NULL;
    return stack->ast;
}


// returns 1 if parser stack matches rule
// checks lookaheads
int parser_rule_match (struct parser_s * parser, int rule, int lookahead)
{
    struct ast_s * ast;
    int depth;
    int i;
    int match;
    
    match = 1;
    for (depth = 0; depth < PARSER_RULES_MAXLEN; depth++) {
        if (MATCH[rule][depth] == -1) {
            break;
        }
        ast = parser_stack_peek(parser, depth);
        if (ast == NULL) {
            match = 0;
            break;
        }
        if (MATCH[rule][depth] != ast->type)
            match = 0;
    }
    
    if (match) {
        for (i = 0; i < PARSER_LOOKAHEAD_MAXLEN; i++) {
            if (LOOKAHEAD[rule][i] == -1)
                break;
            if (LOOKAHEAD[rule][i] == lookahead) {
                match = 0;
                break;
            }
        }
    }    
    
    return match;
}


// returns rule matched, or -1 for no rules matched
int parser_match (struct parser_s * parser, int lookahead)
{
    int rule_i;
    
    for (rule_i = 0; rule_i < PARSER_RULES; rule_i++) {
        if (parser_rule_match(parser, rule_i, lookahead)) {
            return rule_i;
        }
    }
    
    return -1;
}


// returns 1 on a reduction
// 0 on no reduction
int parser_reduce (struct parser_s * parser, int lookahead)
{
    struct ast_s * ast;
    int rule;
    
    rule = parser_match(parser, lookahead);
    
    // SPECIAL CASES
    switch (rule) {
    case RULE_EXPR_PARAMLIST_SYM :
        // make sure we don't have PARAMLIST_SYM_FUNC
        // because then this is a function declaration
        if (parser->stack_size > 2) {
            ast = parser_stack_peek(parser, 2);
            if (ast->type == TOK_FUNC)
                rule = -1;
        }
        break;
    case RULE_STMT_TERM_EXPR :
        // RULE_STMT_TERM_EXPR is designed to allow for one line statements
        // and can only fire if certain things are not beneath it on the stack
        if (parser->stack_size > 2) {
            ast = parser_stack_peek(parser, 2);
            if (    (ast->type == TOK_IF)
                 || (ast->type == TOK_WHILE)
                 || (ast->type == TOK_ADD)
                 || (ast->type == TOK_MINUS)
                 || (ast->type == TOK_STAR)
                 || (ast->type == TOK_DIV)
                 || (ast->type == TOK_MOD)
                 || (ast->type == TOK_GREATER)
                 || (ast->type == TOK_LESS)
                 || (ast->type == TOK_EQUAL)
                )
                rule = -1;
        }
        break;
    }
    
    if (rule == -1)
        return 0;
    
    #if PARSER_DEBUG == 1
        printf("rule: %d\n", rule);
    #endif
    
    switch (rule) {
    case RULE_EXPR_NUM :
    case RULE_EXPR_SYM :
    case RULE_EXPR_STRING :
    case RULE_EXPR_TRUE :
    case RULE_EXPR_FALSE :
        ast = parser_stack_peek(parser, 0);
        ast->subtype = ast->type;
        ast->type = TOK_EXPR;
        break;
    case RULE_EXPR_EXPR_ADD_EXPR :
    case RULE_EXPR_EXPR_MINUS_EXPR :
    case RULE_EXPR_EXPR_STAR_EXPR :
    case RULE_EXPR_EXPR_DIV_EXPR :
    case RULE_EXPR_EXPR_MOD_EXPR :
    case RULE_EXPR_EXPR_LESS_EXPR :
    case RULE_EXPR_EXPR_GREATER_EXPR :
    case RULE_EXPR_EXPR_EQUAL_EXPR :
        ast = parser_stack_peek(parser, 1);
        ast->subtype = ast->type;
        ast->type = TOK_EXPR;
        ast->right = parser_stack_peek(parser, 0);
        ast->left = parser_stack_peek(parser, 2);
        parser_stack_pop(parser, 3);
        parser_stack_push(parser, ast);
        break;
    case RULE_STMT_EXPR_ASSIGN_EXPR :
        ast = ast_create(TOK_STMT, NULL);
        ast->subtype = TOK_ASSIGN;
        ast->right = parser_stack_peek(parser, 0);
        ast->left = parser_stack_peek(parser, 2);
        ast_destroy(parser_stack_peek(parser, 1));
        parser_stack_pop(parser, 3);
        parser_stack_push(parser, ast);
        break;
    case RULE_STMT_TERM_STMT :
    case RULE_PARAMLIST_TERM_PARAMLIST :
    case RULE_IF_TERM_EXPR_IF :
    case RULE_WHILE_TERM_EXPR_WHILE :
    case RULE_ELSE_TERM_ELSE :
        ast_destroy(parser_stack_peek(parser, 0));
        parser_stack_pop(parser, 1);
        break;
    case RULE_STMT_STMT_STMT :
        ast = parser_stack_peek(parser, 1);
        while (ast->next != NULL)
            ast = ast->next;
        ast->next = parser_stack_peek(parser, 0);
        parser_stack_pop(parser, 1);
        break;
    case RULE_STMT_END_STMT_EXPR_IF :
    case RULE_STMT_END_STMT_EXPR_WHILE :
        ast = parser_stack_peek(parser, 3);
        if (ast->type == TOK_IF)
            ast->subtype = TOK_BRANCH;
        else if (ast->type == TOK_WHILE)
            ast->subtype = TOK_LOOP;
        ast->type = TOK_STMT;
        ast->condition = parser_stack_peek(parser, 2);
        ast->block = parser_stack_peek(parser, 1);
        ast_destroy(parser_stack_peek(parser, 0));
        parser_stack_pop(parser, 4);
        parser_stack_push(parser, ast);
        break;
    case RULE_EXPR_PARENC_EXPR_PARENO :
        ast = parser_stack_peek(parser, 1);
        ast_destroy(parser_stack_peek(parser, 0));
        ast_destroy(parser_stack_peek(parser, 2));
        parser_stack_pop(parser, 3);
        parser_stack_push(parser, ast);
        break;
    case RULE_PARAMLIST_PARENC_PARAM_PARENO :
        ast = parser_stack_peek(parser, 1);
        ast->type = TOK_PARAMLIST;
        ast_destroy(parser_stack_peek(parser, 0));
        ast_destroy(parser_stack_peek(parser, 2));
        parser_stack_pop(parser, 3);
        parser_stack_push(parser, ast);
        break;
    case RULE_PARAM_PARENC_EXPR_PARENO_SYM :
        ast = parser_stack_peek(parser, 1);
        ast->type = TOK_PARAM;
        break;
    case RULE_PARAM_EXPR_COMMA_EXPR :
    case RULE_PARAM_EXPR_COMMA_PARAM :
        ast = parser_stack_peek(parser, 1);
        ast = parser_stack_peek(parser, 2);
        ast->type = TOK_PARAM;
        while (ast->next != NULL)
            ast = ast->next;
        ast->next = parser_stack_peek(parser, 0);
        ast->next->type = TOK_PARAM;
        ast = parser_stack_peek(parser, 2);
        ast_destroy(parser_stack_peek(parser, 1));
        parser_stack_pop(parser, 3);
        parser_stack_push(parser, ast);
        break;
    case RULE_EXPR_PARAMLIST_SYM :
        ast = ast_create(TOK_EXPR, NULL);
        ast->subtype = TOK_FUNC;
        ast->left = parser_stack_peek(parser, 1);
        ast->params = parser_stack_peek(parser, 0);
        parser_stack_pop(parser, 2);
        parser_stack_push(parser, ast);
        break;
    case RULE_STMT_END_STMT_PARAMLIST_SYM_FUNC :
        ast = ast_create(TOK_STMT, NULL);
        ast->subtype = TOK_FUNCDEC;
        ast->block = parser_stack_peek(parser, 1);
        ast->params = parser_stack_peek(parser, 2);
        ast->left = parser_stack_peek(parser, 3);
        ast_destroy(parser_stack_peek(parser, 4));
        ast_destroy(parser_stack_peek(parser, 0));
        parser_stack_pop(parser, 5);
        parser_stack_push(parser, ast);
        break;
    case RULE_STMT_EXPR_RETURN :
        ast = parser_stack_peek(parser, 1);
        ast->subtype = TOK_RETURN;
        ast->type = TOK_STMT;
        ast->block = parser_stack_peek(parser, 0);
        parser_stack_pop(parser, 2);
        parser_stack_push(parser, ast);
        break;
    case RULE_STMT_TERM_EXPR :
        ast = parser_stack_peek(parser, 1);
        ast->type = TOK_STMT;
        ast_destroy(parser_stack_peek(parser, 0));
        parser_stack_pop(parser, 1);
        break;
    case RULE_EXPR_PARENC_PARENO_SYM :
        ast = ast_create(TOK_EXPR, NULL);
        ast->subtype = TOK_FUNC;
        ast->left = parser_stack_peek(parser, 2);
        ast->params = NULL;
        ast_destroy(parser_stack_peek(parser, 0));
        ast_destroy(parser_stack_peek(parser, 1));
        parser_stack_pop(parser, 3);
        parser_stack_push(parser, ast);
        break;
    case RULE_ELSEBLOCK_END_STMT_ELSE :
        ast = parser_stack_peek(parser, 2);
        ast->type = TOK_ELSEBLOCK;
        ast->block = parser_stack_peek(parser, 1);
        ast = parser_stack_peek(parser, 0);
        parser_stack_pop(parser, 2);
        parser_stack_push(parser, ast);
        break;
    case RULE_STMT_END_ELSEBLOCK_STMT_EXPR_IF :
        ast = parser_stack_peek(parser, 4);
        ast->subtype = TOK_BRANCH;
        ast->type = TOK_STMT;
        ast->condition = parser_stack_peek(parser, 3);
        ast->block = parser_stack_peek(parser, 2);
        ast->elseblock = parser_stack_peek(parser, 1);
        ast_destroy(parser_stack_peek(parser, 0));
        parser_stack_pop(parser, 4);
        break;
        
    }
    return 1;
}


void parser_stack_debug (struct parser_s * parser)
{
    struct ast_s * ast;
    int depth;
    
    depth = 0;
    while ((ast = parser_stack_peek(parser, depth)) != NULL) {
        depth++;
    }
}


struct parser_s * parser_parse (struct token_s * tokens)
{
    struct parser_s * parser;
    struct token_s * cur;
    struct token_s * next;
    
    parser = (struct parser_s *) malloc(sizeof(struct parser_s));
    parser->bottom = NULL;
    parser->top = NULL;
    parser->ast = NULL;
    parser->stack_size = 0;
    
    cur = tokens;
    while (cur != NULL) {
        parser_stack_push(parser, ast_create(cur->type, cur));
        #if PARSER_DEBUG == 1
            printf("parser stack push: %s => %s\n", tok_debug_string(cur->type), 
                   cur->text);
        #endif
        next = cur->next;
        if (next == NULL) {
            while (parser_reduce(parser, -1))
                parser_stack_debug(parser);
        }
        else {
            while (parser_reduce(parser, next->type))
                parser_stack_debug(parser);
        }
        cur = next;
    }
    
    return parser;
};
