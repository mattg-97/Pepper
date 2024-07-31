#ifndef pepper_ast_h
#define pepper_ast_h

const char* token_literal();

typedef struct {
    const char* (* token_literal)();
} Node;

typedef struct {
    Node node;
} Statement;

typedef struct {
    Node node;
} Expression;

#endif
