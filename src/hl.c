#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "variable.h"

#include "../lib/list.h"
#include "../lib/stack.h"
#include "../lib/standard.h"


char * hl_read_file (char * filename) 
{
    FILE * fh;
    size_t filesize;
    char * text;
    
    fh = fopen(filename, "rb");
    if (fh == NULL) {
        fprintf(stderr, "error opening file %s\n", filename);
        exit(-1);
    }
    fseek(fh, 0, SEEK_END);
    filesize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    fclose(fh);
    
    fh = fopen(filename, "r");
    
    //printf("filesize %d\n", filesize);
    
    text = (char *) malloc(filesize + 1);
    memset(text, 0, filesize + 1);
    fread(text, 1, filesize, fh);
    text[filesize] = (char) 0x00;
    
    fclose(fh);
    
    return text;
}


int main (int argc, char * argv[])
{
    char * text;
    struct token_s * tokens;
    struct parser_s * parser;
    struct in_s * in;

    if (argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(-1);
    }
    
    text = hl_read_file(argv[1]);
    tokens = lexer_lex(text);
    
    /*
    struct token_s * next;
    next = tokens;
    while (next != NULL) {
        printf("%s\n", tok_debug_string(next->type));
        next = next->next;
    }
    */
    
    parser = parser_parse(tokens);
    
    /*
    int i;
    printf("\nprinting AST\n");fflush(stdout);
    for (i = 0; i < parser->stack_size; i++) {
        printf("%d\n", i);
        ast_debug(parser_stack_peek(parser, i));
    }
    */
    
    in = in_create();
    
    lib_standard_register(in);
    lib_list_register(in);
    lib_stack_register(in);
    
    in_exec(in, parser_stack_peek(parser, 0));
    
    in_destroy(in);
    
    free(text);
    
    return 0;
}