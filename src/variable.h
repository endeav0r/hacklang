#ifndef variable_HEADER
#define variable_HEADER

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

#define TYPE_INT  1
#define TYPE_NULL 2
#define TYPE_FUNC 3

struct var_s {
	int type;
	union {
		int i;
        struct ast_s * ast;
	};
	char * string;
};

struct var_s * var_create  (int type, char * value);
void           var_destroy (struct var_s * var);
struct var_s * var_create_func (struct ast_s * ast);

struct var_s * var_add  (struct var_s * a, struct var_s * b);
struct var_s * var_sub  (struct var_s * a, struct var_s * b);
void           var_set  (struct var_s * a, struct var_s * b);
struct var_s * var_copy (struct var_s * src);

int            var_cmp  (struct var_s * a, struct var_s * b);

char *         var_to_string (struct var_s * var);

#endif