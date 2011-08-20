#ifndef interpreter_HEADER
#define interpreter_HEADER

#include "ast.h"
#include "lexer.h"
#include "symboltable.h"
#include "variable.h"

struct in_return_s {
    struct ast_s * ast;
    struct in_return_s * next;
};

struct in_s {
	struct st_s * st;
    struct in_return_s * ret_stack;
};

struct in_s * in_create ();
void          in_destroy (struct in_s * in);

struct var_s * in_stmt    (struct in_s * in, struct ast_s * ast);
int            in_exec    (struct in_s * in, struct ast_s * ast);
struct var_s * in_expr    (struct in_s * in, struct ast_s * expr);
void           in_assign  (struct in_s * in, struct ast_s * ast);
int            in_cond    (struct in_s * in, struct ast_s * ast);
struct var_s * in_call    (struct in_s * in, struct ast_s * ast);
void           in_funcdec (struct in_s * in, struct ast_s * ast);



#endif