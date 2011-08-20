#ifndef lexer_HEADER
#define lexer_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOK_NUM     1
#define TOK_SYM     2
#define TOK_ADD     3
#define TOK_TERM    4
#define TOK_ASSIGN  5
#define TOK_STAR    6
#define TOK_MINUS   7
#define TOK_DIV     8
#define TOK_IF      9
#define TOK_END     10
#define TOK_LESS    11
#define TOK_GREATER 12
#define TOK_WHILE   13
#define TOK_EQUAL   14
#define TOK_PAREN_O 15
#define TOK_PAREN_C 16
#define TOK_COMMA   17
#define TOK_BRACK_O 18
#define TOK_BRACK_C 19
#define TOK_FUNC    20
#define TOK_RETURN  21

struct token_s {
	char * text;
	int type;
	struct token_s * next;
	struct token_s * prev;
};


struct lexer_s {
	struct token_s * first;
	struct token_s * last;
};

struct token_s * lexer_lex (char * text);

void token_destroy (struct token_s * token);

#endif