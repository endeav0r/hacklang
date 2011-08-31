#ifndef capi_HEADER
#define capi_HEADER

#include "interpreter.h"
#include "symboltable.h"
#include "variable.h"

#define CAPI_TYPE_INT     1
#define CAPI_TYPE_NULL    2
#define CAPI_TYPE_UNKNOWN 3
#define CAPI_TYPE_STRING  4
#define CAPI_TYPE_CDATA   5

struct in_s;

struct capi_stack_s {
    struct var_s * var;
    struct capi_stack_s * next;
};


struct capi_s {
    int stack_size;
    struct capi_stack_s * stack;
};


struct var_s * capi_call (struct in_s * in, struct ast_s * ast);

void capi_register_function (struct in_s * in,
                             int (* capi_function) (struct capi_s *),
                             char * symbol);

struct capi_s *       capi_create    ();
void                  capi_destroy   (struct capi_s * capi);
struct capi_stack_s * capi_peek      (struct capi_s * capi, int depth);

// user functions
void                  capi_pop       (struct capi_s * capi);
void                  capi_push      (struct capi_s * capi, struct var_s * var);
void                  capi_push_top  (struct capi_s * capi, struct var_s * var);
int                   capi_size      (struct capi_s * capi);
int                   capi_type      (struct capi_s * capi, int depth);
void *                capi_to_cdata  (struct capi_s * capi, int depth);
int                   capi_to_int    (struct capi_s * capi, int depth);
char *                capi_to_string (struct capi_s * capi, int depth);
struct var_s *        capi_to_var    (struct capi_s * capi, int depth);

#endif