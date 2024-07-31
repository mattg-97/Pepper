#include <string.h>

#include "lexer.h"
#include "logger.h"
#include "debug.h"

// scanner variable
Lexer lexer;

// initialize the scanner with sensible defaults
Lexer* init_lexer(const char* source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
    lexer.tokens = NULL;
    lexer.tokenCapacity = 0;
    lexer.tokenCount = 0;
    return &lexer;
}
void de_init_lexer() {
    for (size_t i = 0; i < lexer.tokenCount; i++) {
        free(lexer.tokens[i].literal);
    }
    free(lexer.tokens);
    lexer.tokens = NULL;
    lexer.tokenCount = 0;
    lexer.tokenCapacity = 0;
}

void add_token(Token token) {
    if (lexer.tokenCount == lexer.tokenCapacity) {
        lexer.tokenCapacity = lexer.tokenCapacity == 0 ? 8 : lexer.tokenCapacity * 2;
        lexer.tokens = realloc(lexer.tokens, lexer.tokenCapacity * sizeof(Token));
        if (lexer.tokens == NULL) {
            ERROR("Failed to allocate memory for tokens.");
            exit(EXIT_FAILURE);
        }
    }
    lexer.tokens[lexer.tokenCount] = token;
    lexer.tokenCount++;
}

static bool is_at_end() {
    return *lexer.current == '\0';
}

static char* literal(Token* token) {
    char* literal = (char*)malloc(token->length + 1);
    if (literal == NULL) {
        ERROR("Unable to allocate memory for token literal.");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < token->length; i++) {
        literal[i] = token->start[i];
    }
    literal[token->length] = '\0';
    return literal;
}

static Token create_token(TokenType type) {
    Token token;
    token.type = type;
    token.length = (size_t)(lexer.current - lexer.start);
    token.line = lexer.line;
    token.start = lexer.start;
    token.literal = literal(&token);
    add_token(token);
    return token;
}

static Token error_token(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.length = (size_t)strlen(message);
    token.line = lexer.line;
    token.start = message;
    token.literal = literal(&token);
    add_token(token);
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

static TokenType check_for_keyword(size_t start, size_t length, const char* rest, TokenType type) {
    if ((size_t)(lexer.current - lexer.start) == start + length
        && memcmp(lexer.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type() {
    switch (lexer.start[0]) {
        case 'a': return check_for_keyword(1, 2, "nd", TOKEN_AND);
        case 'e': {
            if (lexer.current - lexer.start > 1) {
                switch (lexer.start[1]) {
                    case 'n': return check_for_keyword(2, 2, "um", TOKEN_ENUM);
                    case 'l': return check_for_keyword(2, 2, "se", TOKEN_ELSE);
                }
            }
        }
        case 'f': {
            if (lexer.current - lexer.start > 1) {
                switch (lexer.start[1]) {
                    case 'a': return check_for_keyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return check_for_keyword(2, 1, "r", TOKEN_FOR);
                    case 'n': return TOKEN_FN;
                }
            }
        }
        case 'i': return check_for_keyword(1, 1, "f", TOKEN_IF);
        case 'm': {
            if (lexer.current - lexer.start > 1) {
                switch (lexer.start[1]) {
                    case 'u': check_for_keyword(2, 1, "t", TOKEN_MUT);
                    case 'e': check_for_keyword(2, 4, "thod", TOKEN_METHOD);
                }
            }
        }
        case 'n': return check_for_keyword(1, 2, "il", TOKEN_NIL);
        case 'o': return check_for_keyword(1, 1, "r", TOKEN_OR);
        case 'p': return check_for_keyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return check_for_keyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return check_for_keyword(1, 5, "truct", TOKEN_STRUCT);
        case 't': return check_for_keyword(1, 3, "rue", TOKEN_TRUE);
        case 'v': return check_for_keyword(1, 3, "oid", TOKEN_VOID);
        case 'w': return check_for_keyword(1, 4, "hile", TOKEN_WHILE);
        default: return TOKEN_IDENTIFIER;
    }
}

static Token identifier() {
    while (is_alpha(peek()) || is_numeric(peek())) advance();
    return create_token(identifier_type());
}

static Token number() {
    while (is_numeric(peek())) advance();

    if (peek() == '.' && is_numeric(peekNext())) {
        advance();
        while (is_numeric(peek())) advance();
    }

    return create_token(TOKEN_NUMBER);
}

static Token string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') lexer.line++;
        advance();
    }
    if (is_at_end()) return error_token("Unterminated string");

    // for the closing quote
    advance();
    return create_token(TOKEN_STRING);
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
        case '-': return create_token(match('-') ? TOKEN_DECREMENT : TOKEN_MINUS);
        case '+': return create_token(match('+') ? TOKEN_INCREMENT : TOKEN_PLUS);
        case '/': return create_token(TOKEN_SLASH);
        case '*': return create_token(TOKEN_STAR);
        case '?': return create_token(TOKEN_QUESTION_MARK);
        case '@': return create_token(TOKEN_AT);
        case ':': {
            if (match('=')) {
                return create_token(TOKEN_ASSIGN);
            } else if (match(':')) {
                return create_token(TOKEN_COLON_COLON);
            } else {
                return create_token(TOKEN_COLON);
            }
        }
        case ';': return create_token(TOKEN_SEMICOLON);
        case '=': return create_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '"': return string();
        case '!': return create_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '\0': return create_token(TOKEN_EOF);
    }
    return error_token("Unidentified token");
}

void tokenize() {
    Token token = scan_token();
    do {
#ifdef DEBUG_MODE_TOKEN
        debug_token(&token);
#endif
        token = scan_token();
    } while (token.type != TOKEN_EOF);
#ifdef DEBUG_MODE_TOKEN
    debug_token(&token);
#endif
}