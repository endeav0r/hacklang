#include "capi.h"


void capi_register_function (struct in_s * in, 
                             int (* capi_function) (struct capi_stack_s *),
                             char * symbol)
{
    struct var_s * var;
    
    var = var_create_capi_function(capi_function);
    
    st_insert(in->st, var, symbol);
}


/*
struct var_s * capi_call (struct in_s * in, struct ast_s * func_ast)
{
	struct capi_stack_s * capi_stack;

    struct var_s * ret_var;
    struct var_s * param_var;
    struct ast_s * caller_param;
    struct ast_s * callee_param;
    
    // set params
    caller_param = ast->params;
    callee_param = func_ast->params;
    while (callee_param != NULL) {
        if (caller_param == NULL) {
            fprintf(stderr, "not enough arguments supplied to func %s\n",
                    func_ast->left->token->text);
            exit(-1);
        }
        param_var = in_expr(in, caller_param);
        st_insert(in->st, callee_param->token->text, param_var);
        caller_param = caller_param->next;
        callee_param = callee_param->next;
    }
    
    // execute statements and get return value
    ret_var = in_stmt(in, func_ast->block);
    
    // pop symbol table/stack frame
    in->st = st_pop(in->st);
    
    return ret_var;
*/

	
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
	
	return next;
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


int capi_to_int (struct capi_stack_s * capi_stack, int depth) {
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