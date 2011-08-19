#ifndef symboltable_HEADER
#define symboltable_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variable.h"

struct st_s {
	char * symbol;
	struct var_s * var;
	int level;
	struct st_s * left;
	struct st_s * right;
};

struct st_s *  st_insert (struct st_s * tree, char * symbol, struct var_s * var);
struct var_s * st_find   (struct st_s * tree, char * symbol);
struct st_s *  st_delete (struct st_s * tree, char * symbol);

void           st_destroy (struct st_s * tree);

void st_debug (struct st_s * tree);

#endif