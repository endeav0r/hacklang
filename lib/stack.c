#include "stack.h"

void lib_stack_register (struct in_s * in)
{
    capi_register_function(in, lib_stack_create, "stack");
    capi_register_function(in, lib_stack_push,   "stack_push");
    capi_register_function(in, lib_stack_peek,   "stack_peek");
    capi_register_function(in, lib_stack_pop,    "stack_pop");
    capi_register_function(in, lib_stack_size,   "stack_size");
}


struct lib_stack_s * lib_stack_api_create ()
{
    struct lib_stack_s * stack;
    
    stack = (struct lib_stack_s *) malloc(sizeof(struct lib_stack_s));
    stack->type = STACK_TYPE_STACK;
    stack->size = 0;
    stack->stack = NULL;
    
    return stack;
}


void lib_stack_api_destroy (struct lib_stack_s * stack)
{
    struct lib_stack_item_s * item;
    struct lib_stack_item_s * next;
    
    item = stack->stack;
    while (item != NULL) {
        next = item->next;
        var_destroy(item->var);
        free(item);
        item = next;
    }
    
    free(stack);
}


void lib_stack_api_push (struct lib_stack_s * stack, struct var_s * var)
{
    struct lib_stack_item_s * item;
    
    item = (struct lib_stack_item_s *) malloc(sizeof(struct lib_stack_item_s));
    item->next = stack->stack;
    item->var = var;
    stack->stack = item;
    stack->size++;
}


struct var_s * lib_stack_api_peek (struct lib_stack_s * stack)
{
    if (stack == NULL)
    {
        fprintf(stderr, "LIB_STACK_PEEK: TRIED TO PEEK ON NULL STACK\n");
        exit(-1);
    }
    return stack->stack->var;
}


void lib_stack_api_pop (struct lib_stack_s * stack)
{
    struct lib_stack_item_s * item;
    
    if (stack->stack != NULL) {
        item = stack->stack->next;
        var_destroy(stack->stack->var);
        free(stack->stack);
        stack->stack = item;
        stack->size--;
    }
    else {
        fprintf(stderr, "LIB_STACK_POP: TRIED TO POP ON NULL STACK\n");
        exit(-1);
    }   
}


int lib_stack_api_size (struct lib_stack_s * stack)
{
    return stack->size;
}


int lib_stack_create (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_stack_s * stack;
    
    if (capi_size(capi) == 0) {
        stack = lib_stack_api_create();
        var = var_create_cdata(stack, lib_stack_copy, lib_stack_free);
        capi_push(capi, var);
        return 1;
    }
    else {
        fprintf(stderr, "LIB_STACK_STACK: TAKES NO ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_stack_push (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_stack_s * stack;
    
    if (capi_size(capi) == 2) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_STACK_PUSH_0: FIRST ARG MUST BE STACK (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        stack = capi_to_cdata(capi, 0);
        if (stack->type != STACK_TYPE_STACK) {
            fprintf(stderr, "LIB_STACK_PUSH_1: FIRST ARG MUST BE STACK\n");
            exit(-1);
        }
        var = capi_to_var(capi, 1);
        lib_stack_api_push(stack, var_copy(var));
        capi_pop(capi);
        capi_pop(capi);
    }
    else {
        fprintf(stderr, "LIB_STACK_PUSH: TAKES 2 ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_stack_peek (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_stack_s * stack;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_STACK_PEEK_0: FIRST ARG MUST BE STACK (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        stack = capi_to_cdata(capi, 0);
        if (stack->type != STACK_TYPE_STACK) {
            fprintf(stderr, "LIB_STACK_PEEK_1: FIRST ARG MUST BE STACK\n");
            exit(-1);
        }
        var = lib_stack_api_peek(stack);
        capi_pop(capi);
        capi_push(capi, var_copy(var));
    }
    else {
        fprintf(stderr, "LIB_STACK_PEEK: TAKES 1 ARGUMENT (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 1;
}


int lib_stack_pop (struct capi_s * capi)
{
    struct lib_stack_s * stack;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_STACK_PEEK_0: FIRST ARG MUST BE STACK (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        stack = capi_to_cdata(capi, 0);
        if (stack->type != STACK_TYPE_STACK) {
            fprintf(stderr, "LIB_STACK_PEEK_1: FIRST ARG MUST BE STACK\n");
            exit(-1);
        }
        lib_stack_api_pop(stack);
        capi_pop(capi);
    }
    else {
        fprintf(stderr, "LIB_STACK_STACK: TAKES NO ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_stack_size (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_stack_s * stack;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_STACK_PEEK_0: FIRST ARG MUST BE STACK (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        stack = capi_to_cdata(capi, 0);
        if (stack->type != STACK_TYPE_STACK) {
            fprintf(stderr, "LIB_STACK_PEEK_1: FIRST ARG MUST BE STACK\n");
            exit(-1);
        }
        var = var_create(TYPE_INT, NULL);
        var->i = lib_stack_api_size(stack);
        capi_pop(capi);
        capi_push(capi, var);
    }
    else {
        fprintf(stderr, "LIB_STACK_STACK: TAKES NO ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 1;
}


void * lib_stack_copy (void * data)
{
    struct lib_stack_item_s * item;
    struct lib_stack_item_s * item_copy;
    struct lib_stack_s * stack;
    struct lib_stack_s * stack_copy;
    
    stack = (struct lib_stack_s *) data;
    
    stack_copy = lib_stack_api_create();
    
    item = stack->stack;
    if (item != NULL) {
        item_copy = (struct lib_stack_item_s *) malloc(sizeof(struct lib_stack_item_s));
        item_copy->var = var_copy(item->var);
        item_copy->next = NULL;
        stack_copy->stack = item_copy;
        stack_copy->size++;
        
        item = item->next;
        while (item != NULL) {
            item_copy->next = (struct lib_stack_item_s *) malloc(sizeof(struct lib_stack_item_s));
            item_copy = item_copy->next;
            item_copy->var = var_copy(item->var);
            item_copy->next = NULL;
            item = item->next;
            stack_copy->size++;
        }
    }
    
    return stack_copy;
}


void lib_stack_free (void * data)
{
    lib_stack_api_destroy((struct lib_stack_s *) data);
}
    