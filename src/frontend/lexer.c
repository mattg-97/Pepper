#include <ctype.h>
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
void initLexer(const char* source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

static bool isAtEnd() {
    return *lexer.current == '\0';
}

static Token createToken(TokenType type) {
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

void skipWhitespace() {

}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            // we want to be able to have variables such as hello_func
            c == '_';
}

static Token identifier() {
    return createToken(TOKEN_IDENTIFIER);
}

Token scanToken() {
    //skip any whitespace
    skipWhitespace();
    lexer.current = lexer.start;

    if (isAtEnd()) return createToken(TOKEN_EOF);


    char c = advance();
    if (isAlpha(c)) return identifier();
    switch (c) {
        case '{': return createToken(TOKEN_LEFT_BRACE);
        case '}': return createToken(TOKEN_RIGHT_BRACE);
        case '(': return createToken(TOKEN_LEFT_PAREN);
        case ')': return createToken(TOKEN_RIGHT_PAREN);
    }
    return errorToken("Unidentified token.");
}
