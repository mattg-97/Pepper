#include "debug.h"
#include "logger.h"

static const char* print_token_type(TokenType type) {
    switch (type) {
        // Single character tokens.
        case TOKEN_LEFT_BRACE: return "TOKEN_LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "TOKEN_RIGHT_BRACE";
        case TOKEN_RIGHT_PAREN: return "TOKEN_RIGHT_PAREN";
        case TOKEN_LEFT_PAREN: return "TOKEN_LEFT_PAREN";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_DOT: return "TOKEN_DOT";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_SLASH: return "TOKEN_SLASH";
        case TOKEN_STAR: return "TOKEN_STAR";
        case TOKEN_QUESTION_MARK: return "TOKEN_QUESTION_MARK";
        case TOKEN_AT: return "TOKEN_AT";
        // One or two character tokens
        case TOKEN_BANG: return "TOKEN_BANG";
        case TOKEN_BANG_EQUAL: return "TOKEN_BANG_EQUAL";
        case TOKEN_EQUAL: return "TOKEN_EQUAL";
        case TOKEN_EQUAL_EQUAL: return "TOKEN_EQUAL_EQUAL";
        case TOKEN_GREATER: return "TOKEN_GREATER";
        case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";
        case TOKEN_LESS: return "TOKEN_LESS";
        case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_COLON_COLON: return "TOKEN_COLON_COLON";
        case TOKEN_DECREMENT: return "TOKEN_DECREMENT";
        case TOKEN_INCREMENT: return "TOKEN_INCREMENT";
        // Literals
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_NUMBER: return "TOKEN_NUMBER";
        // Keywords
        case TOKEN_AND: return "TOKEN_AND";
        case TOKEN_STRUCT: return "TOKEN_STRUCT";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_FALSE: return "TOKEN_FALSE";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_FN: return "TOKEN_FN";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_NIL: return "TOKEN_NIL";
        case TOKEN_OR: return "TOKEN_OR";
        case TOKEN_PRINT: return "TOKEN_PRINT";
        case TOKEN_RETURN: return "TOKEN_RETURN";
        case TOKEN_TRUE: return "TOKEN_TRUE";
        case TOKEN_LET: return "TOKEN_LET";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_METHOD: return "TOKEN_METHOD";
        case TOKEN_ENUM: return "TOKEN_ENUM";
        case TOKEN_CONST: return "TOKEN_CONST";
        case TOKEN_AS: return "TOKEN_AS";
        case TOKEN_MUT: return "TOKEN_MUT";
        case TOKEN_VOID: return "TOKEN_VOID";

        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_ERROR: return "TOKEN_ERROR";
    }
    return "TOKEN_UNKNOWN";
}

void debug_token(Token* token) {
    const char* type = print_token_type(token->type);
    INFO("Token: |%s|  Type: |%s|", token->literal, type);
}
