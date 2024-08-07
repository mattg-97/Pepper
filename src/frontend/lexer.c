#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "logger.h"
#include "debug.h"


// initialize the scanner with sensible defaults
Lexer* init_lexer(const char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        ERROR("Run out of memory when initializing lexer");
        exit(EXIT_FAILURE);
    }
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->tokens = (Token*)calloc(1, sizeof(Token));
    lexer->token_capacity = 0;
    lexer->token_count = 0;
    return lexer;
}
void de_init_lexer(Lexer* lexer) {
    free(lexer->tokens);
    lexer->tokens = NULL;
    lexer->token_count = 0;
    lexer->token_capacity = 0;
}

static void add_token(Lexer* lexer, Token token) {
    if (lexer->token_count == lexer->token_capacity) {
        lexer->token_capacity = lexer->token_capacity == 0 ? 8 : lexer->token_capacity * 2;
        lexer->tokens = realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
        if (lexer->tokens == NULL) {
            ERROR("Failed to allocate memory for tokens.");
            exit(EXIT_FAILURE);
        }
    }
    lexer->tokens[lexer->token_count] = token;
    lexer->token_count++;
}

static bool is_at_end(Lexer* lexer) {
    return *lexer->current == '\0';
}

static Token create_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.length = (u64)(lexer->current - lexer->start);
    token.line = lexer->line;
    token.start = lexer->start;
    add_token(lexer, token);
    return token;
}

static Token error_token(Lexer* lexer, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.length = (u64)strlen(message);
    token.line = lexer->line;
    token.start = message;
    add_token(lexer, token);
    return token;
}

static char advance(Lexer* lexer) {
    lexer->current++;
    return lexer->current[-1];
}

static bool match(Lexer* lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    lexer->current++;
    return true;
}

static char peek(Lexer* lexer) {
    return *lexer->current;
}

static char peekNext(Lexer* lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->current[1];
}

void skip_whitespace(Lexer* lexer) {
    for (;;) {
        // peek the char
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            case '\n':
                lexer->line++;
                advance(lexer);
                break;
            case '/':
                if (peekNext(lexer) == '/') {
                    while (peek(lexer) != '\n' && !is_at_end(lexer)) advance(lexer);
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

static TokenType check_for_keyword(Lexer* lexer, u64 start, u64 length, const char* rest, TokenType type) {
    if ((u64)(lexer->current - lexer->start) == start + length
        && memcmp(lexer->start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type(Lexer* lexer) {
    switch (lexer->start[0]) {
        case 'a': return check_for_keyword(lexer, 1, 2, "nd", TOKEN_AND);
        case 'e': {
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'n': return check_for_keyword(lexer, 2, 2, "um", TOKEN_ENUM);
                    case 'l': return check_for_keyword(lexer, 2, 2, "se", TOKEN_ELSE);
                }
            }
        }
        case 'f': {
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'a': return check_for_keyword(lexer, 2, 3, "lse", TOKEN_FALSE);
                    case 'o': return check_for_keyword(lexer, 2, 1, "r", TOKEN_FOR);
                    case 'n': return TOKEN_FN;
                }
            }
        }
        case 'i': return check_for_keyword(lexer, 1, 1, "f", TOKEN_IF);
        case 'm': {
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'u': check_for_keyword(lexer, 2, 1, "t", TOKEN_MUT);
                    case 'e': check_for_keyword(lexer, 2, 4, "thod", TOKEN_METHOD);
                }
            }
        }
        case 'n': return check_for_keyword(lexer, 1, 2, "il", TOKEN_NIL);
        case 'o': return check_for_keyword(lexer, 1, 1, "r", TOKEN_OR);
        case 'p': return check_for_keyword(lexer, 1, 4, "rint", TOKEN_PRINT);
        case 'r': return check_for_keyword(lexer, 1, 5, "eturn", TOKEN_RETURN);
        case 's': return check_for_keyword(lexer, 1, 5, "truct", TOKEN_STRUCT);
        case 't': return check_for_keyword(lexer, 1, 3, "rue", TOKEN_TRUE);
        case 'v': return check_for_keyword(lexer, 1, 3, "oid", TOKEN_VOID);
        case 'w': return check_for_keyword(lexer, 1, 4, "hile", TOKEN_WHILE);
        default: return TOKEN_IDENTIFIER;
    }
}

static Token identifier(Lexer* lexer) {
    while (is_alpha(peek(lexer)) || is_numeric(peek(lexer))) advance(lexer);
    return create_token(lexer, identifier_type(lexer));
}

static Token number(Lexer* lexer) {
    while (is_numeric(peek(lexer))) advance(lexer);

    if (peek(lexer) == '.' && is_numeric(peekNext(lexer))) {
        advance(lexer);
        while (is_numeric(peek(lexer))) advance(lexer);
    }

    return create_token(lexer, TOKEN_NUMBER);
}

static Token string(Lexer* lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') lexer->line++;
        advance(lexer);
    }
    if (is_at_end(lexer)) return error_token(lexer, "Unterminated string");

    // for the closing quote
    advance(lexer);
    return create_token(lexer, TOKEN_STRING);
}

Token scan_token(Lexer* lexer) {
    //skip any whitespace
    skip_whitespace(lexer);
    lexer->start = lexer->current;

    if (is_at_end(lexer)) return create_token(lexer, TOKEN_EOF);

    char c = advance(lexer);
    if (is_numeric(c)) return number(lexer);
    if (is_alpha(c)) return identifier(lexer);
    switch (c) {
        case '{': return create_token(lexer, TOKEN_LEFT_BRACE);
        case '}': return create_token(lexer, TOKEN_RIGHT_BRACE);
        case '(': return create_token(lexer, TOKEN_LEFT_PAREN);
        case ')': return create_token(lexer, TOKEN_RIGHT_PAREN);
        case '.': return create_token(lexer, TOKEN_DOT);
        case ',': return create_token(lexer, TOKEN_COMMA);
        case '-': return create_token(lexer, match(lexer, '-') ? TOKEN_DECREMENT : TOKEN_MINUS);
        case '+': return create_token(lexer, match(lexer, '+') ? TOKEN_INCREMENT : TOKEN_PLUS);
        case '/': return create_token(lexer, TOKEN_SLASH);
        case '*': return create_token(lexer, TOKEN_STAR);
        case '?': return create_token(lexer, TOKEN_QUESTION_MARK);
        case '@': return create_token(lexer, TOKEN_AT);
        case '%': return create_token(lexer, TOKEN_PERCENT);
        case ':': {
            if (match(lexer, '=')) {
                return create_token(lexer, TOKEN_ASSIGN);
            } else if (match(lexer, ':')) {
                return create_token(lexer, TOKEN_COLON_COLON);
            } else {
                return create_token(lexer, TOKEN_COLON);
            }
        }
        case ';': return create_token(lexer, TOKEN_SEMICOLON);
        case '=': return create_token(lexer, match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '"': return string(lexer);
        case '!': return create_token(lexer, match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '\0': return create_token(lexer, TOKEN_EOF);
    }
    return error_token(lexer, "Unidentified token");
}

void tokenize(Lexer* lexer) {
    #ifdef DEBUG_MODE_TOKEN
    printf("--- TOKENS ---\n");
    #endif
    Token token = scan_token(lexer);
    do {
#ifdef DEBUG_MODE_TOKEN
        debug_token(&token);
#endif
        token = scan_token(lexer);
    } while (token.type != TOKEN_EOF);
#ifdef DEBUG_MODE_TOKEN
    debug_token(&token);
    printf("--- TOKENS ---\n\n");
#endif
}
