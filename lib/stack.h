#ifndef stack_HEADER
#define stack_HEADER

#include "../src/capi.h"
#include "../src/interpreter.h"
#include "../src/variable.h"

#define STACK_TYPE_STACK 20001

struct lib_stack_item_s {
    struct var_s * var;
    struct lib_stack_item_s * next;
};

struct lib_stack_s {
    int type;
    int size;
    struct lib_stack_item_s * stack;
};

void lib_stack_register (struct in_s * in);


struct lib_stack_s * lib_stack_api_create  ();
void                 lib_stack_api_destroy (struct lib_stack_s * stack);
void                 lib_stack_api_push    (struct lib_stack_s * stack,
                                            struct var_s * var);
struct var_s *       lib_stack_api_peek    (struct lib_stack_s * stack);
void                 lib_stack_api_pop     (struct lib_stack_s * stack);
int                  lib_stack_api_size    (struct lib_stack_s * stack);


int    lib_stack_create (struct capi_s * capi);
int    lib_stack_push   (struct capi_s * capi);
int    lib_stack_peek   (struct capi_s * capi);
int    lib_stack_pop    (struct capi_s * capi);
int    lib_stack_size   (struct capi_s * capi);
void * lib_stack_copy   (void * data);
void   lib_stack_free   (void * data);

#endif