#include "standard.h"

void lib_standard_register (struct in_s * in)
{
    capi_register_function(in, lib_standard_print, "print");
    capi_register_function(in, lib_standard_str, "str");
}


int lib_standard_str (struct capi_s * capi)
{
    struct var_s * var;
    
    if (capi_size(capi) == 0) {
        fprintf(stderr, "LIB_STANDARD_STR: NOARG\n");
        exit(-1);
    }
    else if (capi_size(capi) == 1) {
        var = var_create(TYPE_STRING, capi_to_string(capi, 0));
        capi_pop(capi);
        capi_push(capi, var);
        return 1;
    }
    else {
        fprintf(stderr, "LIB_STANDARD_STR: TOO MANY ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_standard_print (struct capi_s * capi)
{
    if (capi_size(capi) == 0) {
        fprintf(stderr, "LIB_STANDARD_PRINT: NOARG\n");
        exit(-1);
    }
    else if (capi_size(capi) == 1) {
        printf("%s", capi_to_string(capi, 0));
        capi_pop(capi);
    }
    else {
        fprintf(stderr, "LIB_STANDARD_PRINT: TOO MANY ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}