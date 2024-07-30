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
void init_lexer(const char* source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

static bool is_at_end() {
    return *lexer.current == '\0';
}

static Token create_token(TokenType type) {
    Token token;
    token.type = type;
    token.length = (int)(lexer.current - lexer.start);
    token.line = lexer.line;
    token.start = lexer.start;
    return token;
}

static Token error_token(const char* message) {
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
    if (is_at_end()) return false;
    if (*lexer.current != expected) return false;
    lexer.current++;
    return true;
}

static char peek() {
    return *lexer.current;
}

static char peekNext() {
    if (is_at_end()) return '\0';
    return lexer.current[1];
}

void skip_whitespace() {
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
                    while (peek() != '\n' && !is_at_end()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            // we want to be able to have variables such as hello_func
            c == '_';
}

static bool is_numeric(char c) {
    return (c >= '0' && c <= '9');
}

static Token identifier() {
    while (is_alpha(peek()) || is_numeric(peek())) advance();
    return create_token(TOKEN_IDENTIFIER);
}

static Token number() {
    while (is_numeric(peek())) advance();

    if (peek() == '.' && is_numeric(peekNext())) {
        advance();
        while (is_numeric(peek())) advance();
    }

    return create_token(TOKEN_NUMBER);
}

Token scan_token() {
    //skip any whitespace
    skip_whitespace();
    lexer.start = lexer.current;

    if (is_at_end()) return create_token(TOKEN_EOF);

    char c = advance();
    if (is_numeric(c)) return number();
    if (is_alpha(c)) return identifier();
    switch (c) {
        case '{': return create_token(TOKEN_LEFT_BRACE);
        case '}': return create_token(TOKEN_RIGHT_BRACE);
        case '(': return create_token(TOKEN_LEFT_PAREN);
        case ')': return create_token(TOKEN_RIGHT_PAREN);
        case '.': return create_token(TOKEN_DOT);
        case ',': return create_token(TOKEN_COMMA);
        case '-': return create_token(TOKEN_MINUS);
        case '+': return create_token(TOKEN_PLUS);
        case '/': return create_token(TOKEN_SLASH);
        case '*': return create_token(TOKEN_STAR);
        case '?': return create_token(TOKEN_QUESTION_MARK);
        case '@': return create_token(TOKEN_AT);
        case ':': return create_token(match('=') ? TOKEN_ASSIGN : TOKEN_COLON);
        case ';': return create_token(TOKEN_SEMICOLON);
        case '=': return create_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '\0': return create_token(TOKEN_EOF);
    }
    return error_token("Unidentified token!");
}

void tokenize() {
    Token token = scan_token();
    do {
#ifdef DEBUG_MODE
        char* literal = print_token_literal(&token);
        INFO("Token: |%s|  Type: |%s|", literal, print_token_type(token.type));
        free(literal);
#endif
        token = scan_token();
    } while (token.type != TOKEN_EOF);
    char* literal = print_token_literal(&token);
    INFO("Token: |%s|  Type: |%s|", print_token_literal(&token), print_token_type(token.type));
    free(literal);
}