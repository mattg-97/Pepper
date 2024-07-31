#ifndef pepper_ast_h
#define pepper_ast_h

#include "lexer.h"

typedef struct {
} Statement;

typedef struct {
    Token token;
    char* value;
} Identifier;

typedef struct {
    Token token;
    Identifier* name;
    Expression value;
} AssignmentStatement;

typedef struct {
    union {
        AssignmentStatement assignment;
        Identifier identifier;
    } as;
} Expression;

typedef struct {
    union {
        Expression expression;
        Statement statement;
    } as;
} Node;

typedef struct {
    Statement* statements;
    size_t statementCount;
    size_t statementCapacity;
} Program;

#endif