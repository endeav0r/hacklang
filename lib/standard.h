#ifndef standard_HEADER
#define standard_HEADER

#include "../src/interpreter.h"
#include "../src/capi.h"

void lib_standard_register (struct in_s * in);

int lib_standard_print (struct capi_stack_s * capi_stack);

#endif