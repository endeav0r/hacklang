#include "lexer.h"


struct token_s * token_create (char * text, int text_len, int type)
{
    struct token_s * token;
    
    token = (struct token_s *) malloc(sizeof(struct token_s));
    token->text = (char *) malloc(text_len + 1);
    memcpy(token->text, text, text_len);
    token->text[text_len] = 0;
    token->type = type;
    token->next = NULL;
    token->prev = NULL;
    
    return token;
}


void token_destroy (struct token_s * token)
{
    if (token->text != NULL)
        free(token->text);
    if (token->prev != NULL)
        token->prev->next = token->next;
    if (token->next != NULL)
        token->next->prev = token->prev;
    free(token);
}


void lexer_token_append (struct lexer_s * lexer, struct token_s * token)
{
    if (lexer->first == NULL) {
        lexer->first = token;
        lexer->last  = token;
    }
    else {
        lexer->last->next = token;
        token->prev = lexer->last;
        lexer->last = token;
    }
}


int is_alpha (char c)
{
    if (    ((c >= 'A') && (c <= 'Z'))
         || ((c >= 'a') && (c <= 'z'))
         || (c == '_'))
        return 1;
    return 0;
}


int is_numeric (char c)
{
    if ((c >= '0') && (c <= '9'))
        return 1;
    return 0;
}


// returns -1 on no match
int lexer_keyword_match (char * text, int len) {
    switch (len) {
    case 2 :
        if (strcmp("if", text) == 0)
            return TOK_IF;
        break;
    case 3 :
        if (strncmp("end", text, strlen("end")) == 0)
            return TOK_END;
        break;
    case 4 :
        if (strncmp("func", text, len) == 0)
            return TOK_FUNC;
        break;
    case 5 :
        if (strncmp("while", text, strlen("while")) == 0)
            return TOK_WHILE;
        break;
    case 6 :
        if (strncmp("return", text, len) == 0)
            return TOK_RETURN;
        break;
    }
    return -1;
}


struct token_s * lexer_lex (char * text)
{
    int c_i;
    int line = 1;
    int text_i;
    
    struct lexer_s lexer;
    
    lexer.first = NULL;
    lexer.last  = NULL;
    
    text_i = 0;
    while (text_i < strlen(text)) {
        switch (text[text_i]) {
        // match whitespace
        case ' ' :
        case '\t' :
            text_i++;
            continue;
        // match newline
        case '\n' :
            lexer_token_append(&lexer, token_create("\n", 1, TOK_TERM));
            text_i++;
            line++;
            continue;
        case '+' :
            lexer_token_append(&lexer, token_create("+", 1, TOK_ADD));
            text_i++;
            continue;
        case '=' :
            if (text[text_i+1] == '=') {
                lexer_token_append(&lexer, token_create("==", 2, TOK_EQUAL));
                text_i += 1;
            }
            lexer_token_append(&lexer, token_create("=", 1, TOK_ASSIGN));
            text_i++;
            continue;
        case '*' :
            lexer_token_append(&lexer, token_create("*", 1, TOK_STAR));
            text_i++;
            continue;
        case '-' :
            lexer_token_append(&lexer, token_create("-", 1, TOK_MINUS));
            text_i++;
            continue;
        case '/' :
            lexer_token_append(&lexer, token_create("/", 1, TOK_DIV));
            text_i++;
            continue;
        case '<' :
            lexer_token_append(&lexer, token_create("<", 1, TOK_LESS));
            text_i++;
            continue;
        case '>' :
            lexer_token_append(&lexer, token_create(">", 1, TOK_GREATER));
            text_i++;
            continue;
        case '(' :
            lexer_token_append(&lexer, token_create("(", 1, TOK_PAREN_O));
            text_i++;
            continue;
        case ')' :
            lexer_token_append(&lexer, token_create(")", 1, TOK_PAREN_C));
            text_i++;
            continue;
        case ',' :
            lexer_token_append(&lexer, token_create(",", 1, TOK_COMMA));
            text_i++;
            continue;
        case '[' :
            lexer_token_append(&lexer, token_create("[", 1, TOK_BRACK_O));
            text_i++;
            continue;
        case ']' :
            lexer_token_append(&lexer, token_create("]", 1, TOK_BRACK_C));
            text_i++;
            continue;
        }
        // match symbol
        if (is_alpha(text[text_i])) {
            for (c_i = 1; c_i < strlen(text) - text_i; c_i++) {
                if (    (is_alpha  (text[text_i + c_i]) == 0)
                     && (is_numeric(text[text_i + c_i]) == 0))
                    break;
            }
            if (lexer_keyword_match(&(text[text_i]), c_i) >= 0)
                lexer_token_append(&lexer, token_create(&(text[text_i]), c_i,
                                   lexer_keyword_match(&(text[text_i]), c_i)));
            else
                lexer_token_append(&lexer, token_create(&(text[text_i]), c_i,
                                   TOK_SYM));
            text_i += c_i;
            continue;
        }
        // match numeric
        if (is_numeric(text[text_i])) {
            for (c_i = 1; c_i < strlen(text) - text_i; c_i++) {
                if (is_numeric(text[text_i + c_i]) == 0)
                    break;
            }
            lexer_token_append(&lexer, token_create(&(text[text_i]), c_i, TOK_NUM));
            text_i += c_i;
            continue;
        }
        else {
            fprintf(stderr, "UNKNOWN SYMBOL LINE %d\n", line);
            exit(-1);
        }
    }
    
    return lexer.first;
}    