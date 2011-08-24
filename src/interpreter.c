#include "interpreter.h"

struct in_s * in_create ()
{
    struct in_s * in;
    
    in = (struct in_s *) malloc(sizeof(struct in_s));
    in->st = st_create();
    in->ret_stack = NULL;
    
    return in;
}


void in_destroy (struct in_s * in)
{
    if (in->st != NULL)
        st_destroy(in->st);
    free(in);
}


int in_exec (struct in_s * in, struct ast_s * ast)
{
    if (ast->type != TOK_STMT) {
        fprintf(stderr, "interpreter not given statement ast\n");
        exit(-1);
    }
    var_destroy(in_stmt(in, ast));
    
    return 0;
}


struct var_s * in_stmt (struct in_s * in, struct ast_s * ast)
{
    while (ast != NULL) {
        if (ast->subtype == TOK_ASSIGN)
            in_assign(in, ast);
        else if (ast->subtype == TOK_BRANCH) {
            if (in_cond(in, ast->condition))
                in_stmt(in, ast->block);
        }
        else if (ast->subtype == TOK_LOOP) {
            while (in_cond(in, ast->condition)) {
                var_destroy(in_stmt(in, ast->block));
            }
        }
        else if (ast->subtype == TOK_FUNCDEC) {
            in_funcdec(in, ast);
        }
        else if (ast->subtype == TOK_RETURN) {
            return in_expr(in, ast->block);
        }
        else if (ast->subtype == TOK_FUNC) {
            var_destroy(in_call(in, ast));
        }
        else if (ast->block != NULL)
            in_stmt(in, ast->block);
        ast = ast->next;
    }
    return var_create(TYPE_NULL, NULL);
}


int in_cond (struct in_s * in, struct ast_s * ast)
{
    int cmp = 0;
    struct var_s * l;
    struct var_s * r;
    
    l = in_expr(in, ast->left);
    r = in_expr(in, ast->right);
    
    switch (ast->subtype) {
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
    
    switch (ast->subtype) {
    case TOK_ADD :
    case TOK_MINUS :
    case TOK_STAR :
    case TOK_DIV :
    case TOK_MOD :
        a = in_expr(in, ast->left);
        b = in_expr(in, ast->right);
        if (ast->subtype == TOK_ADD)
            r = var_add(a, b);
        else if (ast->subtype == TOK_MINUS)
            r = var_sub(a, b);
        else if (ast->subtype == TOK_STAR)
            r = var_mul(a, b);
        else if (ast->subtype == TOK_DIV)
            r = var_div(a, b);
        else
            r = var_mod(a, b);
        var_destroy(a);
        var_destroy(b);
        break;
    case TOK_NUM :
        r = var_create(TYPE_INT, ast->token->text);
        break;
    case TOK_STRING :
        r = var_create(TYPE_STRING, ast->token->text);
        break;
    case TOK_FUNC :
        r = in_call(in, ast);
        break;
    case TOK_SYM :
        r = st_find(in->st, ast->token->text);
        if (r == NULL)
            r = var_create(TYPE_NULL, "");
        else
            r = var_copy(r);
        break;
    default :
        fprintf(stderr, "in_expr invalid type %d\n", ast->subtype);
        exit(-1);
    }
    return r;
}


void in_assign (struct in_s * in, struct ast_s * ast)
{
    struct var_s * r;
    struct var_s * l;
    
    if (ast->left->subtype != TOK_SYM) {
        fprintf(stderr, "tried to assign to non-symbol, type: %d %s\n",
                ast->left->subtype, ast->left->token->text);
        exit(-1);
    }
    
    l = st_find(in->st, ast->left->token->text);
    if (l == NULL) {
        l = var_create(TYPE_NULL, "");
        st_insert(in->st, ast->left->token->text, l);
    }
    
    r = in_expr(in, ast->right);
    
    var_set(l, r);
    var_destroy(r);
}


struct var_s * in_call (struct in_s * in, struct ast_s * ast) 
{
    struct var_s * ret_var;
    struct var_s * func_var;
    struct var_s * param_var;
    struct ast_s * func_ast;
    struct ast_s * caller_param;
    struct ast_s * callee_param;
    
    // get function from symbol table
    func_var = st_find(in->st, ast->left->token->text);
    if (func_var == NULL) {
        fprintf(stderr, "called non-existant function %s\n",
                ast->left->token->text);
        exit(-1);
    }
    
    func_ast = func_var->ast;
    
    // if this is a capi_call, the capi code will handle it
    if (func_var->type == TYPE_CFUNC) {
        return capi_call(in, ast);
    }
    
    // push symbol table/stack frame
    in->st = st_push(in->st);
    
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
}
    


void in_funcdec (struct in_s * in, struct ast_s * ast)
{
    struct var_s * var;
    struct var_s * new;
    
    var = st_find_scoped(in->st, ast->left->token->text);
    if (var != NULL) {
        new = var_create_func(ast);
        var_set(var, new);
        var_destroy(new);
    }
    else {
        var = var_create_func(ast);
        st_insert(in->st, ast->left->token->text, var);
    }
}