#include "ast.h"


struct ast_s * ast_create (int type, struct token_s * token)
{
	struct ast_s * ast;
	
	ast = (struct ast_s *) malloc(sizeof(struct ast_s));
	ast->type = type;
    ast->subtype = 0;
	ast->token = token;
	ast->left = NULL;
	ast->right = NULL;
	ast->condition = NULL;
	ast->next = NULL;
	ast->block = NULL;
	ast->params = NULL;
	
	return ast;
}


void ast_destroy (struct ast_s * ast)
{
	//if (ast->token != NULL)
		//token_destroy(ast->token);
	free(ast);
}


void ast_debug_print (struct ast_s * ast, int depth, char * prepend, int next)
{
	int i;
	
	if (ast == NULL)
		return;
	
	for (i = 0; i < depth; i++)
		printf(" ");
	printf("%s", prepend);
    if (ast->subtype >= 0)
        printf("(%s)", tok_debug_string(ast->subtype));
	if (ast->token != NULL) {
		printf("%s\ttoken=%d,%p,%p,%p,%p ", tok_debug_string(ast->type),
               ast->token->type, ast->token, ast->token->text, ast->token->next, ast->token->prev);
        printf("%s\n", ast->token->text);
    }
	else
		printf("%d\n", ast->type);
	ast_debug_print(ast->params, depth + 1, "(", next);
	ast_debug_print(ast->condition, depth + 1, "?", next);
	ast_debug_print(ast->left, depth + 1, "<", next);
	ast_debug_print(ast->right, depth + 1, ">", next);
	ast_debug_print(ast->block, depth + 1, "[", next);
    if (next)
        ast_debug_print(ast->next, depth, "&", next);
}


void ast_debug (struct ast_s * ast)
{
	ast_debug_print(ast, 1, "", 1);
}


void ast_debug_no_next (struct ast_s * ast)
{
    ast_debug_print(ast, 1, "", 0);
}