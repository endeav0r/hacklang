#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "variable.h"


char * hl_read_file (char * filename) 
{
    FILE * fh;
    size_t filesize;
    char * text;
    
    fh = fopen(filename, "r");
    if (fh == NULL) {
        fprintf(stderr, "error opening file %s\n", filename);
        exit(-1);
    }
    
    fseek(fh, 0, SEEK_END);
    filesize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    printf("filesize %d\n", filesize);
    
    text = (char *) malloc(filesize + 1);
    memset(text, 0, filesize + 1);
    
    fread(text, 1, filesize, fh);
    
    fclose(fh);
    
    return text;
}


int main (int argc, char * argv[])
{

    int i;
    char * text;
    struct token_s * tokens;
    struct token_s * next;
    struct parser_s * parser;
    struct in_s * in;

    if (argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(-1);
    }
    
    text = hl_read_file(argv[1]);
    printf("lexing\n");fflush(stdout);
    tokens = lexer_lex(text);    
    next = tokens;
    while (next != NULL) {
        printf("%s %s\n", tok_debug_string(next->type), next->text);
        next = next->next;
    }
    printf("parsing\n");fflush(stdout);
    parser = parser_parse(tokens);
    printf("\nprinting AST\n");fflush(stdout);
    for (i = 0; i < parser->stack_size; i++) {
        printf("%d\n", i);
        ast_debug(parser_stack_peek(parser, i));
    }
    
    printf("\nin_create\n");fflush(stdout);
    in = in_create();
    in_exec(in, parser_stack_peek(parser, 0));
    
    st_debug(in->st);
    
    printf("in_destroy\n");fflush(stdout);
    in_destroy(in);
    
    free(text);
    
    return 0;
}