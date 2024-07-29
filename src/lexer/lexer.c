#include <string.h>

#include "lexer.h"

typedef struct {
    // marks the beginning of the current lexeme (word) being scanned
    const char* start;
    // the current character being looked at
    const char* current;
    // the current line number we are scanning
    int line;
} Lexer;

// scanner variable
Lexer lexer;

// initialize the scanner with sensible defaults
void initScanner(const char* source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

static bool isAtEnd() {
    return *lexer.current == '\0';
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.length = (int)(lexer.current - lexer.start);
    token.line = lexer.line;
    token.start = lexer.current;
    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.length = (int)strlen(message);
    token.line = lexer.line;
    token.start = message;
    return token;
}

static char advance() {
    lexer.current++;
    return lexer.current[-1];
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*lexer.current != expected) return false;
    lexer.current++;
    return true;
}

static char peek() {
    return *lexer.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return lexer.current[1];
}
