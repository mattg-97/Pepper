#include "debug.h"

const char* print_token_type(TokenType type) {
    switch (type) {
        case TOKEN_AND: return "TOKEN_AND";
        case TOKEN_LEFT_BRACE: return "TOKEN_LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "TOKEN_RIGHT_BRACE";
        case TOKEN_RIGHT_PAREN: return "TOKEN_RIGHT_PAREN";
        case TOKEN_LEFT_PAREN: return "TOKEN_LEFT_PAREN";
    }
    return "TOKEN_UNKNOWN";
}
char* print_token_literal(Token* token) {
    char* literal = (char*)malloc(token->length + 1);
    if (literal == NULL) {
        return NULL;
    }
    for (int i = 0; i < token->length; i++) {
        literal[i] = token->start[i];
    }
    literal[token->length] = '\0';
    return literal;
}