#ifndef variable_HEADER
#define variable_HEADER

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "capi.h"

#define TYPE_INT    1
#define TYPE_NULL   2
#define TYPE_FUNC   3
#define TYPE_CFUNC  4
#define TYPE_STRING 5
#define TYPE_CDATA  6
#define TYPE_BOOL   7

#define TRUE  0
#define FALSE 1

#define VAR_CACHE_SIZE 32

struct capi_s;

struct var_cdata_s {
    int ref_count;
    void * data;
    void * (* copy) (void * data);
    void   (* free) (void * data);
};

struct var_s {
    int type;
    union {
        int i;
        int bool;
        struct ast_s * ast;
        int (* capi_function) (struct capi_s * capi_stack);
        struct var_cdata_s * cdata;
    };
    char * string;
};

struct var_s * var_create               (int type, char * value);
void           var_destroy              (struct var_s * var);
struct var_s * var_create_func          (struct ast_s * ast);
struct var_s * var_create_capi_function (int (*capi_function) (struct capi_s *));
struct var_s * var_create_cdata         (void * data,
                                         void * (* cdata_copy) (void * data),
                                         void (* cdata_free) (void * data));

struct var_s * var_add  (struct var_s * a, struct var_s * b);
struct var_s * var_sub  (struct var_s * a, struct var_s * b);
struct var_s * var_mul  (struct var_s * a, struct var_s * b);
struct var_s * var_div  (struct var_s * a, struct var_s * b);
struct var_s * var_mod  (struct var_s * a, struct var_s * b);

void           var_set  (struct var_s * a, struct var_s * b);
struct var_s * var_copy (struct var_s * src);

int            var_cmp  (struct var_s * a, struct var_s * b);

char *         var_to_string (struct var_s * var);

#endif