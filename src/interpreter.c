#include "interpreter.h"



struct in_s * in_create ()
{
	struct in_s * in;
	
	in = (struct in_s *) malloc(sizeof(struct in_s));
	in->st = NULL;
	
	return in;
}


void in_destroy (struct in_s * in)
{
	st_destroy(in->st);
}


int in_exec (struct in_s * in, struct ast_s * ast)
{
	if (ast->type != TOK_STMT) {
		fprintf(stderr, "interpreter not given statement ast\n");
		exit(-1);
	}
	in_stmt(in, ast);
	return 0;
}


void in_stmt (struct in_s * in, struct ast_s * ast)
{
	while (ast != NULL) {
		if (ast->block->type == TOK_ASSIGN)
			in_assign(in, ast->block);
		else if (ast->left->type == TOK_IF) {
			if (in_cond(in, ast->condition))
				in_stmt(in, ast->block);
		}
		else if (ast->left->type == TOK_WHILE) {
			while (in_cond(in, ast->condition))
				in_stmt(in, ast->block);
		}
		else if (ast->block != NULL)
			in_stmt(in, ast->block);
		ast = ast->next;
	}
}


int in_cond (struct in_s * in, struct ast_s * ast)
{
	int cmp = 0;
	struct var_s * l;
	struct var_s * r;
	
	l = in_expr(in, ast->left);
	r = in_expr(in, ast->right);
	
	switch (ast->token->type) {
	case TOK_LESS :
		if (var_cmp(l, r) < 0)
			cmp = 1;
		else
			cmp = 0;
		break;
	case TOK_GREATER :
		if (var_cmp(l, r) > 0)
			cmp = 1;
		else
			cmp = 0;
		break;
	case TOK_EQUAL :
		if (var_cmp(l, r) == 0)
			cmp = 1;
		else
			cmp = 0;
		break;
	}
	
	var_destroy(l);
	var_destroy(r);
	
	return cmp;
}
			


struct var_s * in_expr (struct in_s * in, struct ast_s * ast)
{
	struct var_s * a;
	struct var_s * b;
	struct var_s * r;
	
	if (ast->token->type == TOK_ADD) {
		a = in_expr(in, ast->left);
		b = in_expr(in, ast->right);
		r = var_add(a, b);
		var_destroy(a);
		var_destroy(b);
	}
	else if (ast->token->type == TOK_MINUS) {
		a = in_expr(in, ast->left);
		b = in_expr(in, ast->right);
		r = var_sub(a, b);
		var_destroy(a);
		var_destroy(b);
	}
	else if (ast->token->type == TOK_NUM) {
		r = var_create(TYPE_INT, ast->token->text);
		return r;
	}
	else if (ast->token->type == TOK_SYM) {
		r = st_find(in->st, ast->token->text);
		if (r == NULL) {
			r = var_create(TYPE_NULL, "");
			in->st = st_insert(in->st, ast->token->text, r);
		}
		r = var_copy(r);
	}
	else {
		fprintf(stderr, "in_expr invalid type %d\n", ast->token->type);
		exit(-1);
	}
	return r;
}


void in_assign (struct in_s * in, struct ast_s * ast)
{
	struct var_s * r;
	struct var_s * l;
	
	if (ast->left->token->type != TOK_SYM) {
		fprintf(stderr, "tried to assign to non-symbol, type: %d\n", ast->left->token->type);
		exit(-1);
	}
	
	l = st_find(in->st, ast->left->token->text);
	if (l == NULL) {
		l = var_create(TYPE_NULL, "");
		in->st = st_insert(in->st, ast->left->token->text, l);
	}
	
	r = in_expr(in, ast->right);
	
	var_set(l, r);
	var_destroy(r);
}
	