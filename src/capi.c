#include "capi.h"


void capi_register_function (struct in_s * in, 
                             int (* capi_function) (struct capi_stack_s *),
                             char * symbol)
{
    struct var_s * var;
    
    var = var_create_capi_function(capi_function);
    
    st_insert(in->st, symbol, var);
}


struct var_s * capi_call (struct in_s * in, struct ast_s * ast)
{
	int capi_ret_val;
	struct capi_stack_s * capi_stack;
    struct var_s * ret_var;
    struct var_s * param_var;
	struct var_s * func_var;
    struct ast_s * func_ast;
    struct ast_s * caller_param;
	
	capi_stack = NULL;
    
    // get function from symbol table
    func_var = st_find(in->st, ast->left->token->text);
    if (func_var == NULL) {
        fprintf(stderr, "called non-existant function %s\n",
                ast->left->token->text);
        exit(-1);
    }
    
    func_ast = func_var->ast;
    
    // push params onto stack
    caller_param = ast->params;
    while (caller_param != NULL) {
        param_var = in_expr(in, caller_param);
		capi_stack = capi_stack_push(capi_stack, param_var);
	    caller_param = caller_param->next;
    }
    
	capi_ret_val = func_var->capi_function(capi_stack);
	
	if (capi_ret_val == 1)
		ret_var = capi_stack->var;
	else
		ret_var = var_create(TYPE_NULL, NULL);
    
	return ret_var;
}

	
struct capi_stack_s * capi_stack_create ()
{
	return NULL;
}


void capi_stack_destroy (struct capi_stack_s * capi_stack)
{
	if (capi_stack->next != NULL)
		capi_stack_destroy(capi_stack->next);
	var_destroy(capi_stack->var);
	free(capi_stack);
}


struct capi_stack_s * capi_stack_push (struct capi_stack_s * capi_stack,
                                       struct var_s * var) 
{
	struct capi_stack_s * new_stack;
	
	new_stack = (struct capi_stack_s *) malloc(sizeof(struct capi_stack_s));
	new_stack->var = var;
	new_stack->next = capi_stack;

	return new_stack;
}


struct capi_stack_s * capi_stack_pop (struct capi_stack_s * capi_stack)
{
	struct capi_stack_s * ret;
	
	if (capi_stack == NULL)
		return NULL;
	
	ret = capi_stack->next;
	var_destroy(capi_stack->var);
	free(capi_stack);
	
	return ret;
}


int capi_stack_size (struct capi_stack_s * capi_stack)
{
	struct capi_stack_s * next;
	int size = 0;
	
	next = capi_stack;
	
	while (next != NULL) {
		size++;
		next = next->next;
	}
	
	return size;
}


int capi_stack_type (struct capi_stack_s * capi_stack, int depth)
{
	struct capi_stack_s * next;
	
	next = capi_stack;
	
	while (depth-- > 0) {
		if (next == NULL)
			break;
		next = next->next;
	}
	
	if (next == NULL)
		return CAPI_TYPE_UNKNOWN;
	
	switch (capi_stack->var->type) {
	case TYPE_INT :
		return CAPI_TYPE_INT;
	case TYPE_NULL :
		return CAPI_TYPE_NULL;
	}
	
	return CAPI_TYPE_UNKNOWN;
}


int capi_stack_to_int (struct capi_stack_s * capi_stack, int depth) {
	struct capi_stack_s * next;
	
	next = capi_stack;
	
	while (depth-- > 0) {
		if (next == NULL)
			break;
		next = next->next;
	}
	
	if (next == NULL)
		return 0;
	
	return next->var->i;
}