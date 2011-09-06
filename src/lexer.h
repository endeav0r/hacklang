#ifndef lexer_HEADER
#define lexer_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOK_NUM     0x01
#define TOK_SYM     0x02
#define TOK_STRING  0x03
#define TOK_TERM    0x04
#define TOK_ASSIGN  0x05
#define TOK_TRUE    0x06
#define TOK_FALSE   0x07
#define TOK_PAREN_O 0x08
#define TOK_PAREN_C 0x09

#define TOK_ADD     0x10
#define TOK_MINUS   0x11
#define TOK_STAR    0x12
#define TOK_DIV     0x13
#define TOK_MOD     0x14

#define TOK_IF      0x20
#define TOK_ELSE    0x21
#define TOK_ELSIF   0x22
#define TOK_WHILE   0x23
#define TOK_FUNC    0x24
#define TOK_RETURN  0x25
#define TOK_END     0x26

#define TOK_GREATER 0x30
#define TOK_LESS    0x31
#define TOK_EQUAL   0x32
#define TOK_NEQUAL  0x33

#define TOK_COMMA   0x40
#define TOK_BRACK_O 0x41
#define TOK_BRACK_C 0x42

#define TOK_INVALID 0x43

struct token_s {
    char * text;
    int type;
    int line;
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