#include "variable.h"

struct var_s * var_create (int type, char * value) {
    struct var_s * var;
    
    var = (struct var_s *) malloc(sizeof(struct var_s));
    
    var->type = type;
    var->string = NULL;
    switch (type) {
        case TYPE_INT :
            if (value == NULL)
                var->i = 0;
            else
                var->i = strtoul(value, NULL, 0);
            break;
    }
    
    return var;
}


void var_destroy (struct var_s * var)
{
    if (var->string != NULL) {
        free(var->string);
    }
    free(var);
}


struct var_s * var_create_func (struct ast_s * ast) {
    struct var_s * var;
    
    var = (struct var_s *) malloc(sizeof(struct var_s));
    var->type = TYPE_FUNC;
    var->ast = ast;
    var->string = NULL;
    
    return var;
}


struct var_s * var_create_capi_function (int (*capi_function)
                                         (struct capi_stack_s *)) {
    struct var_s * var;
    
    var = var_create(TYPE_CFUNC, NULL);
    var->capi_function = capi_function;
    
    return var;
}


struct var_s * var_add (struct var_s * a, struct var_s * b)
{
    struct var_s * r;
    
    r = var_create(TYPE_NULL, NULL);
    if (a->type == b->type) {
        switch (a->type) {
        case TYPE_INT :
            r->type = TYPE_INT;
            r->i = a->i + b->i;
            break;
        default :
            r->type = TYPE_NULL;
            break;
        }
    }
    else {
        fprintf(stderr, "adding incompatible types %d %d\n", a->type, b->type);
        exit(-1);
    }
    
    return r;
}


struct var_s * var_sub (struct var_s * a, struct var_s * b)
{
    struct var_s * r;
    
    r = var_create(TYPE_NULL, NULL);
    if (a->type == b->type) {
        switch (a->type) {
        case TYPE_INT :
            r->type = TYPE_INT;
            r->i = a->i - b->i;
            break;
        default :
            r->type = TYPE_NULL;
            break;
        }
    }
    else {
        fprintf(stderr, "subtracting incompatible types %d %d\n", a->type, b->type);
        exit(-1);
    }
    
    return r;
}


struct var_s * var_mul (struct var_s * a, struct var_s * b)
{
	struct var_s * r;
	
	r = var_create(TYPE_NULL, NULL);
	if ((a->type == TYPE_INT) && (b->type == TYPE_INT)) {
		r->type = TYPE_INT;
		r->i = a->i * b->i;
	}
	else {
		fprintf(stderr, "multiplying incompatible types %d %d\n", a->type, b->type);
		exit(-1);
	}
	
	return r;
}


struct var_s * var_div (struct var_s * a, struct var_s * b)
{
	struct var_s * r;
	
	r = var_create(TYPE_NULL, NULL);
	if ((a->type == TYPE_INT) && (b->type == TYPE_INT)) {
		r->type = TYPE_INT;
		r->i = a->i / b->i;
	}
	else {
		fprintf(stderr, "dividing incompatible types %d %d\n", a->type, b->type);
		exit(-1);
	}
	
	return r;
}


struct var_s * var_mod (struct var_s * a, struct var_s * b)
{
	struct var_s * r;
	
	r = var_create(TYPE_NULL, NULL);
	if ((a->type == TYPE_INT) && (b->type == TYPE_INT)) {
		r->type = TYPE_INT;
		r->i = a->i % b->i;
	}
	else {
		fprintf(stderr, "modulocombobing incompatible types %d %d\n", a->type, b->type);
		exit(-1);
	}
	
	return r;
}


void var_set (struct var_s * a, struct var_s * b)
{
    switch (b->type) {
    case TYPE_INT :
        a->type = TYPE_INT;
        a->i = b->i;
        break;
    case TYPE_NULL :
        a->type = TYPE_NULL;
        break;
    case TYPE_FUNC :
        a->type = TYPE_FUNC;
        a->ast = b->ast;
        break;
    case TYPE_CFUNC :
        a->type = TYPE_CFUNC;
        a->capi_function = b->capi_function;
        break;
    }
}


struct var_s * var_copy (struct var_s * src)
{
    struct var_s * r;
    
    switch (src->type) {
    case TYPE_INT :
        r = var_create(TYPE_INT, var_to_string(src));
        break;
    case TYPE_NULL :
        r = var_create(TYPE_NULL, NULL);
        break;
    case TYPE_FUNC :
        r = var_create_func(src->ast);
        break;
    case TYPE_CFUNC :
        r = var_create_capi_function (src->capi_function);
        break;
    default :
        fprintf(stderr, "var_copy on invalid type %d\n", src->type);
        exit(-1);
    }
    
    return r;
}


int var_cmp (struct var_s * a, struct var_s * b)
{
    if (a->type != b->type) {
        fprintf(stderr, "comparing variables of different type\n");
        exit(-1);
    }
    
    switch (a->type) {
    case TYPE_INT :
        if (a->i < b->i)
            return -1;
        else if (a->i > b->i)
            return 1;
        return 0;
    case TYPE_NULL :
        return 0;
    case TYPE_FUNC :
        if (a->ast == b->ast)
            return 0;
        return 1;
    case TYPE_CFUNC :
        if (a->capi_function == b->capi_function)
            return 0;
        return 1;
    }
    
    fprintf(stderr, "tried to compare invalid type %d %d\n", a->type, b->type);
    exit(-1);
    return 0;
}

    
char * var_to_string (struct var_s * var)
{
    if (var->string != NULL)
        free(var->string);
    var->string = NULL;
        
    switch (var->type) {
    case TYPE_INT :
        var->string = (char *) malloc(24);
        snprintf(var->string, 24, "%d", var->i);
        break;
    case TYPE_NULL :
        var->string = (char *) malloc(8);
        snprintf(var->string, 8, "<NULL>");
        break;
    case TYPE_FUNC :
        var->string = (char *) malloc(64);
        snprintf(var->string, 64, "<function at %p>", var->ast);
        break;
    case TYPE_CFUNC :
        var->string = (char *) malloc(64);
        snprintf(var->string, 64, "<capi_function at %p>", var->capi_function);
        break;
    default :
        fprintf(stderr, "var_to_string on invalid type %d\n", var->type);
        exit(-1);
    }
    
    return var->string;
}