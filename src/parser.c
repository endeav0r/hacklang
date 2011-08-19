#include "parser.h"

// reverse order
int MATCH [PARSER_RULES][PARSER_RULES_MAXLEN] = {
/* 00 */ {TOK_NUM, -1},
/* 01 */ {TOK_EXPR, TOK_ADD,    TOK_EXPR, -1},
/* 02 */ {TOK_EXPR, TOK_ASSIGN, TOK_EXPR, -1},
/* 03 */ {TOK_SYM, -1},
/* 04 */ {TOK_TERM, TOK_STMT, -1},
/* 05 */ {TOK_STMT, TOK_STMT, -1},
/* 06 */ {TOK_EXPR, TOK_MINUS, TOK_EXPR, -1},
/* 07 */ {TOK_EXPR, TOK_STAR, TOK_EXPR, -1},
/* 08 */ {TOK_EXPR, TOK_DIV, TOK_EXPR, -1},
/* 09 */ {TOK_EXPR, TOK_LESS, TOK_EXPR, -1},
/* 10 */ {TOK_EXPR, TOK_GREATER, TOK_EXPR, -1},
/* 11 */ {TOK_END, TOK_STMT, TOK_COND, TOK_IF, -1},
/* 12 */ {TOK_TERM, TOK_COND, -1},
/* 13 */ {TOK_END, TOK_STMT, TOK_COND, TOK_WHILE, -1},
/* 14 */ {TOK_PAREN_C, TOK_EXPR, TOK_PAREN_O, TOK_SYM, -1},
/* 15 */ {TOK_PAREN_C, TOK_EXPR, TOK_PAREN_O, -1},
/* 16 */ {TOK_EXPR, TOK_COMMA, TOK_EXPR, -1},
/* 17 */ {TOK_PAREN_C, TOK_PARAM, TOK_PAREN_O, -1},
/* 18 */ {TOK_EXPR, TOK_COMMA, TOK_PARAM, -1},
/* 19 */ {TOK_END, TOK_STMT, TOK_PARAM, TOK_SYM, -1}
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
/* 09 */ {-1},
/* 10 */ {-1},
/* 11 */ {-1},
/* 12 */ {-1},
/* 13 */ {-1},
/* 14 */ {-1},
/* 15 */ {-1},
/* 16 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 17 */ {-1},
/* 18 */ {TOK_PAREN_O, TOK_ADD, TOK_MINUS, TOK_STAR, TOK_DIV, -1},
/* 19 */ {-1}
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
	if (rule == -1)
		return 0;
	
	switch (rule) {
	case RULE_EXPR_NUM :
	case RULE_EXPR_SYM :
		ast = parser_stack_peek(parser, 0);
		ast->type = TOK_EXPR;
		break;
	case RULE_EXPR_EXPR_ADD_EXPR :
	case RULE_EXPR_EXPR_MINUS_EXPR :
	case RULE_EXPR_EXPR_STAR_EXPR :
	case RULE_EXPR_EXPR_DIV_EXPR :
		ast = parser_stack_peek(parser, 1);
		ast->type = TOK_EXPR;
		ast->right = parser_stack_peek(parser, 0);
		ast->left = parser_stack_peek(parser, 2);
		parser_stack_pop(parser, 3);
		parser_stack_push(parser, ast);
		break;
	case RULE_STMT_EXPR_ASSIGN_EXPR :
		ast = ast_create(TOK_STMT, NULL);
		ast->block = parser_stack_peek(parser, 1);
		ast->block->right = parser_stack_peek(parser, 0);
		ast->block->left = parser_stack_peek(parser, 2);
		parser_stack_pop(parser, 3);
		parser_stack_push(parser, ast);
		break;
	case RULE_STMT_TERM_STMT :
	case RULE_COND_TERM_COND :
		parser_stack_pop(parser, 1);
		break;
	case RULE_STMT_STMT_STMT :
		ast = parser_stack_peek(parser, 1);
		while (ast->next != NULL)
			ast = ast->next;
		ast->next = parser_stack_peek(parser, 0);
		parser_stack_pop(parser, 1);
		break;
	case RULE_COND_EXPR_LESS_EXPR :
	case RULE_COND_EXPR_GREATER_EXPR :
		ast = parser_stack_peek(parser, 1);
		ast->type = TOK_COND;
		ast->left = parser_stack_peek(parser, 2);
		ast->right = parser_stack_peek(parser, 0);
		parser_stack_pop(parser, 3);
		parser_stack_push(parser, ast);
		break;
	case RULE_STMT_END_STMT_COND_IF :
	case RULE_STMT_END_STMT_COND_WHILE :
		ast = ast_create(TOK_STMT, NULL);
		ast->condition = parser_stack_peek(parser, 2);
		ast->block = parser_stack_peek(parser, 1);
		ast->left  = parser_stack_peek(parser, 3);
		ast_destroy(parser_stack_peek(parser, 0));
		parser_stack_pop(parser, 4);
		parser_stack_push(parser, ast);
		break;
	case RULE_EXPR_PARENC_EXPR_PARENO :
	case RULE_PARAM_PARENC_PARAM_PARENO :
		ast = parser_stack_peek(parser, 1);
		ast_destroy(parser_stack_peek(parser, 0));
		ast_destroy(parser_stack_peek(parser, 2));
		parser_stack_pop(parser, 3);
		parser_stack_push(parser, ast);
		break;
	case RULE_PARAM_PARENC_EXPR_PARENO_SYM :
		ast = parser_stack_peek(parser, 1);
		ast->type = TOK_PARAM;
		ast_destroy(parser_stack_peek(parser, 0));
		ast_destroy(parser_stack_peek(parser, 2));
		parser_stack_pop(parser, 3);
		parser_stack_push(parser, ast);
		break;
	case RULE_PARAM_EXPR_COMMA_EXPR :
	case RULE_PARAM_EXPR_COMMA_PARAM :
		ast = parser_stack_peek(parser, 2);
		ast->type = TOK_PARAM;
		ast->next = parser_stack_peek(parser, 0);
		ast->next->type = TOK_PARAM;
		ast_destroy(parser_stack_peek(parser, 1));
		parser_stack_pop(parser, 3);
		parser_stack_push(parser, ast);
		break;
	case RULE_CALL_END_STMT_PARAM_SYM :
		ast = ast_create(TOK_CALL, NULL);
		ast->left = parser_stack_peek(parser, 3);
		ast->right = parser_stack_peek(parser, 2);
		ast->block = parser_stack_peek(parser, 1);
		ast_destroy(parser_stack_peek(parser, 0));
		parser_stack_pop(parser, 4);
		parser_stack_push(parser, ast);
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
	struct token_s * next;
	
	parser = (struct parser_s *) malloc(sizeof(struct parser_s));
	parser->bottom = NULL;
	parser->top = NULL;
	parser->ast = NULL;
	parser->stack_size = 0;
	
	next = tokens;
	while (next != NULL) {
		parser_stack_push(parser, ast_create(next->type, next));
		if (next->next == NULL) {
			while (parser_reduce(parser, -1))
				parser_stack_debug(parser);
		}
		else {
			while (parser_reduce(parser, next->next->type))
				parser_stack_debug(parser);
		}
		next = next->next;
	}
	
	return parser;
};
