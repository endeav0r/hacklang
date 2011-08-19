#ifndef ast_HEADER
#define ast_HEADER

#include <stdlib.h>

#include "lexer.h"

// "tokens" specifically for abstract syntax tree
#define TOK_EXPR   1000
#define TOK_STMT   1001
#define TOK_COND   1002
#define TOK_BRANCH 1003
#define TOK_PARAM  1004
#define TOK_CALL   1005

struct ast_s {
	int type;
	struct token_s * token;
	struct ast_s * left;
	struct ast_s * right;
	struct ast_s * condition;
	struct ast_s * next;
	struct ast_s * block;
};

struct ast_s * ast_create (int type, struct token_s * token);
void           ast_destroy (struct ast_s * ast);

void ast_debug (struct ast_s * ast);

#endif