#ifndef list_HEADER
#define list_HEADER

#include "../src/capi.h"
#include "../src/interpreter.h"
#include "../src/variable.h"

#define LIST_TYPE_LIST 10001
#define LIST_TYPE_ITER 10002

struct lib_list_item_s {
    struct var_s * var;
    struct lib_list_item_s * prev;
    struct lib_list_item_s * next;
};

struct lib_list_s {
    int type;
    int size;
    struct lib_list_item_s * first;
    struct lib_list_item_s * last;
};

struct lib_list_iter_s {
    int type;
    struct lib_list_item_s * cur;
};

void lib_list_register (struct in_s * in);

int    lib_list_create (struct capi_s * capi);
int    lib_list_append (struct capi_s * capi);
int    lib_list_size   (struct capi_s * capi);
void * lib_list_copy   (void * data);
void   lib_list_free   (void * data);

int    lib_list_iter_create   (struct capi_s * capi);
int    lib_list_iter_next     (struct capi_s * capi);
int    lib_list_iter_continue (struct capi_s * capi);
void * lib_list_iter_copy (void * data);
void   lib_list_iter_free (void * data);

#endif