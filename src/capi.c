#include "capi.h"


void capi_register_function (struct in_s * in, 
                             int (* capi_function) (struct capi_s *),
                             char * symbol)
{
    struct var_s * var;
    
    var = var_create_capi_function(capi_function);
    
    st_insert(in->st, symbol, var);
}


struct capi_s * capi_create ()
{
    struct capi_s * capi;
    
    capi = (struct capi_s *) malloc(sizeof(struct capi_s));
    capi->stack_size = 0;
    capi->stack = NULL;
    
    return capi;
}


void capi_destroy (struct capi_s * capi)
{
    struct capi_stack_s * stack;
    struct capi_stack_s * next;
    
    stack = capi->stack;
    while (stack != NULL) {
        next = stack->next;
        var_destroy(stack->var);
        free(stack);
        stack = next;
    }
    
    free(capi);
}


struct var_s * capi_call (struct in_s * in, struct ast_s * ast)
{
    int capi_ret_val;
    struct capi_s * capi;
    struct var_s * ret_var;
    struct var_s * param_var;
    struct var_s * func_var;
    struct ast_s * func_ast;
    struct ast_s * caller_param;
    
    capi = capi_create();
    
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
        capi_push_top(capi, param_var);
        caller_param = caller_param->next;
    }
    
    capi_ret_val = func_var->capi_function(capi);
    if (capi_ret_val == -1) {
        fprintf(stderr, "line: %d\n", ast->token->line);
        exit(-1);
    }
    else if (capi_ret_val == 1)
        ret_var = capi->stack->var;
    else
        ret_var = var_create(TYPE_NULL, NULL);
    
    return ret_var;
}


void capi_push (struct capi_s * capi, struct var_s * var) 
{
    struct capi_stack_s * new_stack;
    
    new_stack = (struct capi_stack_s *) malloc(sizeof(struct capi_stack_s));
    new_stack->var = var;
    new_stack->next = capi->stack;
    capi->stack = new_stack;
    capi->stack_size++;
}


void capi_push_top (struct capi_s * capi, struct var_s * var) 
{
    struct capi_stack_s * new_stack;
    struct capi_stack_s * next;
    
    new_stack = (struct capi_stack_s *) malloc(sizeof(struct capi_stack_s));
    new_stack->var = var;
    new_stack->next = NULL;
    
    if (capi->stack == NULL)
        capi->stack = new_stack;
    else {
        next = capi->stack;
        while (next->next != NULL)
            next = next->next;
        next->next = new_stack;
    }
    capi->stack_size++;
}


void capi_pop (struct capi_s * capi)
{
    struct capi_stack_s * ret;
    
    if (capi->stack == NULL) {
        fprintf(stderr, "tried to pop from null capi stack\n");
        exit(-1);
    }
    
    ret = capi->stack->next;
    var_destroy(capi->stack->var);
    free(capi->stack);
    capi->stack = ret;
    capi->stack_size--;
}


int capi_size (struct capi_s * capi)
{
    return capi->stack_size;
}


int capi_type (struct capi_s * capi, int depth)
{
    struct capi_stack_s * stack;
    
    stack = capi_peek(capi, depth);
    
    if (capi == NULL)
        return CAPI_TYPE_UNKNOWN;
    
    switch (stack->var->type) {
    case TYPE_INT :
        return CAPI_TYPE_INT;
    case TYPE_NULL :
        return CAPI_TYPE_NULL;
    case TYPE_STRING :
        return CAPI_TYPE_STRING;
    case TYPE_CDATA :
        return CAPI_TYPE_CDATA;
    }
    
    return CAPI_TYPE_UNKNOWN;
}


struct capi_stack_s * capi_peek (struct capi_s * capi, int depth)
{
    struct capi_stack_s * next;
    
    next = capi->stack;
    while (depth-- > 0) {
        if (next == NULL)
            break;
        next = next->next;
    }
    
    return next;
}


int capi_to_int (struct capi_s * capi, int depth)
{
    struct capi_stack_s * stack;
    
    stack = capi_peek(capi, depth);
    if (stack == NULL) {
        fprintf(stderr, "capi_stack_to_int at invalid depth\n");
        exit(-1);
    }
    
    return stack->var->i;
}


char * capi_to_string (struct capi_s * capi, int depth)
{
    struct capi_stack_s * stack;
    
    stack = capi_peek(capi, depth);
    if (stack == NULL) {
        fprintf(stderr, "capi_stack_to_str at invalid depth\n");
        exit(-1);
    }
    
    return var_to_string (stack->var);
}


void * capi_to_cdata (struct capi_s * capi, int depth)
{
    struct capi_stack_s * stack;
    
    stack = capi_peek(capi, depth);
    if (stack == NULL) {
        fprintf(stderr, "capi_stack_to_cdata at invalid depth\n");
        exit(-1);
    }
    
    return stack->var->cdata->data;
}


struct var_s * capi_to_var (struct capi_s * capi, int depth)
{
    struct capi_stack_s * stack;
    
    stack = capi_peek(capi, depth);
    if (stack == NULL) {
        fprintf(stderr, "capi_stack_to_var at invalid depth\n");
        exit(-1);
    }
    
    return stack->var;
}