#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "variable.h"

#include "../lib/standard.h"


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
    
    text = (char *) malloc(filesize + 2);
    memset(text, 0, filesize + 2);
    
    fread(text, 1, filesize, fh);
    text[filesize] = '\n';
    text[filesize + 1] = (char) 0;
    
    fclose(fh);
    
    return text;
}


int main (int argc, char * argv[])
{

    int i;
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
    parser = parser_parse(tokens);
    printf("\nprinting AST\n");fflush(stdout);
    for (i = 0; i < parser->stack_size; i++) {
        printf("%d\n", i);
        ast_debug(parser_stack_peek(parser, i));
    }
    
    in = in_create();
    
    lib_standard_register(in);
    
    in_exec(in, parser_stack_peek(parser, 0));
    
    st_debug(in->st);
    
    in_destroy(in);
    
    free(text);
    
    return 0;
}