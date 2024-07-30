#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "../utils/logger.h"
#include "../utils/debug.h"

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
    token.start = lexer.start;
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
    for (;;) {
        // peek the char
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                lexer.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
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
    lexer.start = lexer.current;

    if (isAtEnd()) return createToken(TOKEN_EOF);

    char c = advance();
    //if (isAlpha(c)) return identifier();
    switch (c) {
        case '{': return createToken(TOKEN_LEFT_BRACE);
        case '}': return createToken(TOKEN_RIGHT_BRACE);
        case '(': return createToken(TOKEN_LEFT_PAREN);
        case ')': return createToken(TOKEN_RIGHT_PAREN);
        case '.': return createToken(TOKEN_DOT);
        case ',': return createToken(TOKEN_COMMA);
        case '-': return createToken(TOKEN_MINUS);
        case '+': return createToken(TOKEN_PLUS);
        case ';': return createToken(TOKEN_SEMICOLON);
        case '/': return createToken(TOKEN_SLASH);
        case '*': return createToken(TOKEN_STAR);
        case '?': return createToken(TOKEN_QUESTION_MARK);
        case '@': return createToken(TOKEN_AT);
        case 0: return createToken(TOKEN_EOF);
    }
    return errorToken("Unidentified token!");
}

void scanTokens() {
    Token token = scanToken();
    while (token.type != TOKEN_EOF) {
        INFO("Token: |%s|  Type: |%s|", print_token_literal(&token), print_token_type(token.type));
        token = scanToken();
    }
}