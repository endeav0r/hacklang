#ifndef standard_HEADER
#define standard_HEADER

#include <stdio.h>
#include <string.h>

#include "../src/interpreter.h"
#include "../src/capi.h"

void lib_standard_register (struct in_s * in);

int lib_standard_fread  (struct capi_s * capi);
int lib_standard_print  (struct capi_s * capi);
int lib_standard_str    (struct capi_s * capi);
int lib_standard_strlen (struct capi_s * capi);

#endif