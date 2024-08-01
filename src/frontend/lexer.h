#ifndef pepper_lexer_h
#define pepper_lexer_h

#include "common.h"

typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_COLON, TOKEN_SLASH, TOKEN_STAR,
  TOKEN_QUESTION_MARK, TOKEN_AT,
  // One or two character tokens.
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,
  TOKEN_ASSIGN, TOKEN_COLON_COLON,
  TOKEN_DECREMENT, TOKEN_INCREMENT,
  // Literals.
  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
  // Keywords.
  TOKEN_AND, TOKEN_STRUCT, TOKEN_ELSE, TOKEN_FALSE,
  TOKEN_FOR, TOKEN_FN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
  TOKEN_PRINT, TOKEN_RETURN,
  TOKEN_TRUE, TOKEN_LET, TOKEN_WHILE, TOKEN_METHOD,
  TOKEN_ENUM, TOKEN_CONST, TOKEN_AS,
  TOKEN_MUT, TOKEN_VOID,

  TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    size_t length;
    size_t line;
    char* literal;
} Token;

typedef struct {
    // marks the beginning of the current lexeme (word) being scanned
    const char* start;
    // the current character being looked at
    const char* current;
    // the current line number we are scanning
    size_t line;
    Token* tokens;
    size_t token_count;
    size_t token_capacity;
} Lexer;

Lexer* init_lexer(const char* source);
void de_init_lexer(Lexer* lexer);
Token scan_token(Lexer* lexer);
void tokenize(Lexer* lexer);

#endif
