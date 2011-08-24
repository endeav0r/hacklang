#ifndef capi_HEADER
#define capi_HEADER

#include "interpreter.h"
#include "symboltable.h"
#include "variable.h"

#define CAPI_TYPE_INT     1
#define CAPI_TYPE_NULL    2
#define CAPI_TYPE_UNKNOWN 3
#define CAPI_TYPE_STRING  4

struct in_s;

struct capi_stack_s {
    struct var_s * var;
    struct capi_stack_s * next;
};


struct var_s * capi_call (struct in_s * in, struct ast_s * ast);

void capi_register_function (struct in_s * in,
                             int (* capi_function) (struct capi_stack_s *),
                             char * symbol);

struct capi_stack_s * capi_stack_create    ();
void                  capi_stack_destroy   (struct capi_stack_s * capi_stack);
struct capi_stack_s * capi_stack_push      (struct capi_stack_s * capi_stack, struct var_s * var);
struct capi_stack_s * capi_stack_pop       (struct capi_stack_s * capi_stack);
int                   capi_stack_size      (struct capi_stack_s * capi_stack);
int                   capi_stack_type      (struct capi_stack_s * capi_stack, int depth);
int                   capi_stack_to_int    (struct capi_stack_s * capi_stack, int depth);
char *                capi_stack_to_string (struct capi_stack_s * capi_stack, int depth);

#endif