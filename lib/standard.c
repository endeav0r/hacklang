#include "standard.h"

void lib_standard_register (struct in_s * in)
{
    capi_register_function(in, lib_standard_print, "print");
    capi_register_function(in, lib_standard_str, "str");
}


int lib_standard_str (struct capi_stack_s * capi_stack)
{
    struct var_s * var;
    
    if (capi_stack_size(capi_stack) == 0) {
        fprintf(stderr, "LIB_STANDARD_STR: NOARG\n");
        exit(-1);
    }
    else if (capi_stack_size(capi_stack) == 1) {
        var = var_create(TYPE_STRING, capi_stack_to_string(capi_stack, 0));
        capi_stack = capi_stack_pop(capi_stack);
        capi_stack = capi_stack_push(capi_stack, var);
        return 1;
    }
    else {
        fprintf(stderr, "LIB_STANDARD_STR: TOO MANY ARGUMENTS (%d)\n",
                capi_stack_size(capi_stack));
        exit(-1);
    }
    
    return 0;
}


int lib_standard_print (struct capi_stack_s * capi_stack)
{
    if (capi_stack_size(capi_stack) == 0) {
        fprintf(stderr, "LIB_STANDARD_PRINT: NOARG\n");
        exit(-1);
    }
    else if (capi_stack_size(capi_stack) == 1) {
        printf("%s", capi_stack_to_string(capi_stack, 0));
        capi_stack_pop(capi_stack);
    }
    else {
        fprintf(stderr, "LIB_STANDARD_PRINT: TOO MANY ARGUMENTS (%d)\n",
                capi_stack_size(capi_stack));
        exit(-1);
    }
    
    return 0;
}