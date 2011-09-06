#include "lexer.h"


struct token_s * token_create (char * text, int text_len, int type, int line)
{
    struct token_s * token;
    
    token = (struct token_s *) malloc(sizeof(struct token_s));
    token->text = (char *) malloc(text_len + 1);
    memcpy(token->text, text, text_len);
    token->text[text_len] = 0;
    token->type = type;
    token->line = line;
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
int lexer_keyword_match (char * text, int len)
{
    switch (len) {
    case 2 :
        if (strncmp("if", text, len) == 0)
            return TOK_IF;
        break;
    case 3 :
        if (strncmp("end", text, len) == 0)
            return TOK_END;
        break;
    case 4 :
        if (strncmp("func", text, len) == 0)
            return TOK_FUNC;
        else if (strncmp("true", text, len) == 0)
            return TOK_TRUE;
        else if (strncmp("else", text, len) == 0)
            return TOK_ELSE;
        break;
    case 5 :
        if (strncmp("while", text, len) == 0)
            return TOK_WHILE;
        else if (strncmp("false", text, len) == 0)
            return TOK_FALSE;
        else if (strncmp("elsif", text, len) == 0)
            return TOK_ELSIF;
        break;
    case 6 :
        if (strncmp("return", text, len) == 0)
            return TOK_RETURN;
        break;
    }
    return -1;
}


// helper functions for dealing with strings like stacks
//  (as crazy as that sounds)
// strings are malloced/realloced in 8 + 1 byte chunks, and pushed on to
//  from there
char * lexer_string_push (char * string, char c)
{
    int len;
    
    if (string == NULL) {
        string = (char *) malloc(33);
        string[0] = 0;
        len = 0;
    }
    else if ((len = strlen(string)) % 32 == 0)
        string = realloc(string, len + 33);
    
    string[len] = c;
    string[len + 1] = 0;
    
    return string;
}


struct token_s * lexer_lex (char * text)
{
    int in_string;
    char * string_buf;
    int c_i;
    int line = 1;
    int text_i;
    int strlen_text;
    
    int comment = 0;
    
    struct lexer_s lexer;
    
    lexer.first = NULL;
    lexer.last  = NULL;
    
    text_i = 0;
    in_string = 0;
    string_buf = NULL;
    strlen_text = strlen(text);
    while (text_i < strlen_text) {
        // string handling
        if (in_string) {
            if (text[text_i] == '\\') {
                switch (text[text_i + 1]) {
                case '\\' :
                    string_buf = lexer_string_push(string_buf, '\\');
                    text_i += 2;
                    continue;
                case 'n' :
                    string_buf = lexer_string_push(string_buf, '\n');
                    text_i += 2;
                    continue;
                case 't' :
                    string_buf = lexer_string_push(string_buf, '\t');
                    text_i += 2;
                    continue;
                case '"' :
                    string_buf = lexer_string_push(string_buf, '"');
                    text_i += 2;
                    continue;
                default :
                    fprintf(stderr, "invalid string escape line %d\n", line);
                    exit(-1);
                }
            }
            else if (text[text_i] == '"') {
                in_string = 0;
                if (string_buf == NULL)
                    lexer_token_append(&lexer, token_create("", 0, TOK_STRING, line));
                else
                    lexer_token_append(&lexer,
                                       token_create(string_buf, strlen(string_buf),
                                                    TOK_STRING, line));
                free(string_buf);
                string_buf = NULL;
                text_i++;
                continue;
            }
            else {
                string_buf = lexer_string_push(string_buf, text[text_i]);
                text_i++;
                continue;
            }
        }
        
        if (comment) {
            if (text[text_i] == '\n')
                comment = 0;
            text_i++;
            continue;
        }
        else if (text[text_i] == '#') {
            comment = 1;
            text_i++;
            continue;
        }
        
        if (text[text_i] == '"') {
            in_string = 1;
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
                lexer_token_append(&lexer,
                                   token_create(&(text[text_i]), c_i,
                                                lexer_keyword_match(&(text[text_i]), c_i), 
                                                line));
            else
                lexer_token_append(&lexer, token_create(&(text[text_i]), c_i,
                                                        TOK_SYM, line));
            text_i += c_i;
            continue;
        }
        // match numeric
        if ((text[text_i] == '-') || (is_numeric(text[text_i]))) {
            for (c_i = 1; c_i < strlen(text) - text_i; c_i++) {
                if (is_numeric(text[text_i + c_i]) == 0)
                    break;
            }
            lexer_token_append(&lexer, token_create(&(text[text_i]), c_i,
                                                    TOK_NUM, line));
            text_i += c_i;
            continue;
        }
        
        switch (text[text_i]) {
        // match whitespace
        case ' ' :
        case '\t' :
            text_i++;
            continue;
        case ';' :
        case '\n' :
            lexer_token_append(&lexer, token_create("\n", 1, TOK_TERM, line));
            text_i++;
            line++;
            continue;
        case '+' :
            lexer_token_append(&lexer, token_create("+", 1, TOK_ADD, line));
            text_i++;
            continue;
        case '=' :
            if (text[text_i+1] == '=') {
                lexer_token_append(&lexer, token_create("==", 2, TOK_EQUAL, line));
                text_i += 2;
            }
            else {
                lexer_token_append(&lexer, token_create("=", 1, TOK_ASSIGN, line));
                text_i++;
            }
            continue;
        case '!' :
            if (text[text_i+1] == '=') {
                lexer_token_append(&lexer, token_create("!=", 2, TOK_NEQUAL, line));
                text_i += 2;
                continue;
            }
        case '*' :
            lexer_token_append(&lexer, token_create("*", 1, TOK_STAR, line));
            text_i++;
            continue;
        case '-' :
            if ((text[text_i+1] > '9') && (text[text_i+1] < '0')) {
                lexer_token_append(&lexer, token_create("-", 1, TOK_MINUS, line));
                text_i++;
                continue;
            }
            break;
        case '/' :
            lexer_token_append(&lexer, token_create("/", 1, TOK_DIV, line));
            text_i++;
            continue;
        case '<' :
            lexer_token_append(&lexer, token_create("<", 1, TOK_LESS, line));
            text_i++;
            continue;
        case '>' :
            lexer_token_append(&lexer, token_create(">", 1, TOK_GREATER, line));
            text_i++;
            continue;
        case '(' :
            lexer_token_append(&lexer, token_create("(", 1, TOK_PAREN_O, line));
            text_i++;
            continue;
        case ')' :
            lexer_token_append(&lexer, token_create(")", 1, TOK_PAREN_C, line));
            text_i++;
            continue;
        case ',' :
            lexer_token_append(&lexer, token_create(",", 1, TOK_COMMA, line));
            text_i++;
            continue;
        case '[' :
            lexer_token_append(&lexer, token_create("[", 1, TOK_BRACK_O, line));
            text_i++;
            continue;
        case ']' :
            lexer_token_append(&lexer, token_create("]", 1, TOK_BRACK_C, line));
            text_i++;
            continue;
        case '%' :
            lexer_token_append(&lexer, token_create("%", 1, TOK_MOD, line));
            text_i++;
            continue;
        }
        fprintf(stderr, "UNKNOWN SYMBOL LINE %d\n", line);
        exit(-1);
    }
    
    lexer_token_append(&lexer, token_create("\n", 1, TOK_TERM, line));
    
    return lexer.first;
}    