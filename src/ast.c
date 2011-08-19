#include "ast.h"


struct ast_s * ast_create (int type, struct token_s * token)
{
	struct ast_s * ast;
	
	ast = (struct ast_s *) malloc(sizeof(struct ast_s));
	ast->type = type;
	ast->token = token;
	ast->left = NULL;
	ast->right = NULL;
	ast->condition = NULL;
	ast->next = NULL;
	ast->block = NULL;
	
	return ast;
}


void ast_destroy (struct ast_s * ast) 
{
	token_destroy(ast->token);
	free(ast);
}


void ast_debug_print (struct ast_s * ast, int depth)
{
	int i;
	
	if (ast == NULL)
		return;
	
	for (i = 0; i < depth; i++)
		printf(" ");
	if (ast->token != NULL)
		printf("%d\t%s\n", ast->type, ast->token->text);
	else
		printf("%d\n", ast->type);
	ast_debug_print(ast->condition, depth + 1);
	ast_debug_print(ast->left, depth + 1);
	ast_debug_print(ast->right, depth + 1);
}


void ast_debug (struct ast_s * ast)
{
	ast_debug_print(ast, 0);
}