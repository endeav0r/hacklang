#include "variable.h"

struct var_s * var_create (int type, char * value)
{
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
    case TYPE_STRING :
        var->string = (char *) malloc(strlen(value) + 1);
        strcpy(var->string, value);
        break;
    }
    
    return var;
}


void var_destroy (struct var_s * var)
{
    if (var->string != NULL) {
        free(var->string);
    }
    if (var->type == TYPE_CDATA) {
        var->cdata->ref_count--;
        if (var->cdata->ref_count == 0) {
            var->cdata->free(var->cdata->data);
            free(var->cdata);
            free(var);
        }
    }
    else
        free(var);
}


struct var_s * var_create_func (struct ast_s * ast)
{
    struct var_s * var;
    
    var = var_create(TYPE_FUNC, NULL);
    var->type = TYPE_FUNC;
    var->ast = ast;
    
    return var;
}


struct var_s * var_create_capi_function (int (*capi_function)
                                         (struct capi_s *))
{
    struct var_s * var;
    
    var = var_create(TYPE_CFUNC, NULL);
    var->capi_function = capi_function;
    
    return var;
}


struct var_s * var_create_cdata (void * data,
                                 void * (* copy) (void * data),
                                 void (* free) (void * data))
{
    struct var_s * var;
    struct var_cdata_s * cdata;
    
    var = var_create(TYPE_CDATA, NULL);
    cdata = (struct var_cdata_s *) malloc(sizeof(struct var_cdata_s));
    
    cdata->ref_count = 1;
    cdata->data = data;
    cdata->copy = copy;
    cdata->free = free;
    
    var->cdata = cdata;
    
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
        case TYPE_STRING :
            r->type = TYPE_STRING;
            r->string = (char *) malloc(strlen(a->string) + strlen(b->string) + 1);
            strcpy(r->string, a->string);
            strcat(r->string, b->string);
            break;
        default :
            fprintf(stderr, "adding incompatible types %d %d\n", a->type, b->type);
            exit(-1);
            break;
        }
    }
    else {
        fprintf(stderr, "adding different types %d %d\n", a->type, b->type);
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
    int i;
    struct var_s * r;
    
    r = var_create(TYPE_NULL, NULL);
    if ((a->type == TYPE_INT) && (b->type == TYPE_INT)) {
        r->type = TYPE_INT;
        r->i = a->i * b->i;
    }
    else if ((a->type == TYPE_STRING) && (b->type == TYPE_INT)) {
        r->type = TYPE_STRING;
        r->string = (char *) malloc(strlen(a->string) * b->i + 1);
        r->string[0] = (char) 0;
        for (i = 0; i < b->i; i++) {
            strcat(r->string, a->string);
        }
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
    case TYPE_STRING :
        a->type = TYPE_STRING;
        if (a->string != NULL)
            free(a->string);
        a->string = (char *) malloc(strlen(b->string) + 1);
        strcpy(a->string, b->string);
        break;
    case TYPE_CDATA :
        a->type = TYPE_CDATA;
        a->cdata = b->cdata;
        a->cdata->ref_count++;
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
    case TYPE_STRING :
        r = var_create(TYPE_STRING, src->string);
        break;
    case TYPE_CDATA :
        r = var_create(TYPE_CDATA, NULL);
        r->cdata = src->cdata;
        r->cdata->ref_count++;
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
        return a->i - b->i;
    case TYPE_NULL :
        return 0;
    case TYPE_FUNC :
        return a->ast - b->ast;
    case TYPE_CFUNC :
        return a->capi_function - b->capi_function;
    case TYPE_STRING :
        return strcmp(a->string, b->string);
    case TYPE_CDATA :
        return a->cdata - b->cdata;
    }
    
    fprintf(stderr, "tried to compare invalid type %d %d\n", a->type, b->type);
    exit(-1);
    return 0;
}

    
char * var_to_string (struct var_s * var)
{
    if (var->type == TYPE_STRING)
        return var->string;
        
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
    case TYPE_CDATA :
        var->string = (char *) malloc(64);
        snprintf(var->string, 64, "<cdata at %p>", var->cdata);
        break;
    default :
        fprintf(stderr, "var_to_string on invalid type %d\n", var->type);
        exit(-1);
    }
    
    return var->string;
}