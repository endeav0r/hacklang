#include "standard.h"

void lib_standard_register (struct in_s * in)
{
	capi_register_function(in, lib_standard_print, "print");
}


int lib_standard_print (struct capi_stack_s * capi_stack)
{

	int i;

	if (capi_stack_size(capi_stack) == 0) {
		printf("LIB_STANDARD_PRINT: NOARG\n");
	}
	else if (capi_stack_size(capi_stack) == 1) {
		if (capi_stack_type(capi_stack, 0) == CAPI_TYPE_INT) {
			i = capi_stack_to_int (capi_stack, 0);
			printf("LIB_STANDARD_PRINT: %d\n", i);
		}
		else {
			printf("lib_standard_print given invalid type %d\n",
			       capi_stack_type(capi_stack, 0));
	    }
		capi_stack_pop(capi_stack);
	}
	
	return 0;
}