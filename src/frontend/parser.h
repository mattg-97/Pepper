#ifndef pepper_parser_h
#define pepper_parser_h

#include "lexer.h"
#include "ast.h"

typedef struct {
    Token* tokens;
    Token currentToken;
    Token peekToken;
    size_t current;
} Parser;

Parser* init_parser(Token* tokens);
void de_init_parser();
void next_token();
Program* parse_program();

#endif
