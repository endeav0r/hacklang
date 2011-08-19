#include "variable.h"

struct var_s * var_create (int type, char * value) {
	struct var_s * var;
	
	var = (struct var_s *) malloc(sizeof(struct var_s));
	
	var->type = type;
	var->string = NULL;
	switch (type) {
		case TYPE_INT :
			if (value == NULL)
				var->i = 0;
			else
				var->i = strtoul(value, NULL, 0);
			break;
	}
	
	return var;
}


void var_destroy (struct var_s * var)
{
	free(var);
}


struct var_s * var_add (struct var_s * a, struct var_s * b)
{
	struct var_s * r;
	
	r = (struct var_s *) malloc(sizeof(struct var_s));
	if (a->type == b->type) {
		switch (a->type) {
		case TYPE_INT :
			r->type = TYPE_INT;
			r->i = a->i + b->i;
			break;
		default :
			r->type = TYPE_NULL;
			break;
		}
	}
	else {
		fprintf(stderr, "adding incompatible types %d %d\n", a->type, b->type);
		exit(-1);
	}
	
	return r;
}


struct var_s * var_sub (struct var_s * a, struct var_s * b)
{
	struct var_s * r;
	
	r = (struct var_s *) malloc(sizeof(struct var_s));
	if (a->type == b->type) {
		switch (a->type) {
		case TYPE_INT :
			r->type = TYPE_INT;
			r->i = a->i - b->i;
			break;
		default :
			r->type = TYPE_NULL;
			break;
		}
	}
	else {
		fprintf(stderr, "subtractinging incompatible types %d %d\n", a->type, b->type);
		exit(-1);
	}
	
	return r;
}


void var_set (struct var_s * a, struct var_s * b)
{
	if (b->type == TYPE_INT) {
		a->type = TYPE_INT;
		a->i = b->i;
	}
}


struct var_s * var_copy (struct var_s * src)
{
	struct var_s * r;
	
	switch (src->type) {
	case TYPE_INT :
		r = var_create(TYPE_INT, var_to_string(src));
		break;
	case TYPE_NULL :
		r = var_create(TYPE_NULL, NULL);
		break;
	default :
		fprintf(stderr, "var_copy on invalid type %d\n", src->type);
		exit(-1);
	}
	
	return r;
}


int var_cmp (struct var_s * a, struct var_s * b)
{
	if (a->type != b->type) {
		fprintf(stderr, "comparing variables of different type\n");
		exit(-1);
	}
	
	switch (a->type) {
	case TYPE_INT :
		if (a->i < b->i)
			return -1;
		else if (a->i > b->i)
			return 1;
		return 0;
	}
	
	fprintf(stderr, "tried to compare invalid type %d\n", a->type);
	exit(-1);
	return 0;
}

	
char * var_to_string (struct var_s * var)
{
	if (var->string != NULL)
		free(var->string);
		
	switch (var->type) {
	case TYPE_INT :
		var->string = (char *) malloc(24);
		snprintf(var->string, 24, "%d", var->i);
		break;
	case TYPE_NULL :
		var->string = (char *) malloc(5);
		snprintf(var->string, 5, "NULL");
		break;
	default :
		fprintf(stderr, "var_to_string on invalid type %d\n", var->type);
		exit(-1);
	}
	
	return var->string;
}