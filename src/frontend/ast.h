#ifndef pepper_ast_h
#define pepper_ast_h

#include "lexer.h"

typedef struct {
} Statement;

/*typedef struct {
    Token token;
    char* value;
} Identifier;

typedef struct {
    Token token;
    Identifier* name;
} AssignmentStatement;


typedef struct {
    union {
        Statement statement;
    } as;
} Node;*/

typedef struct {
    Statement* statements;
    size_t statementCount;
    size_t statementCapacity;
} Program;

// VISITOR PATTERN

struct Visitor;

typedef struct Expression {
    void (*accept)(struct Visitor*, struct Expression*);
} Expression;

typedef struct Visitor {
    void (*visitBinary)(struct Visitor*, struct Expression*);
    void (*visit_literal)(struct Visitor*, struct Expression*);
} Visitor;

typedef struct {
    Expression base;
    int value;
} Literal;

Literal init_literal(int value);
void accept_literal(Expression* expression, Visitor* visitor);

typedef struct {
    Expression base;
    Expression* left;
    TokenType operator;
    Expression* right;
} Binary;

typedef struct {
    Visitor base;
} ConcreteVisitor;

Binary init_binary(Expression* left, TokenType operator, Expression* right);
ConcreteVisitor* init_visitor();
void acceptBinary(Expression* expression, Visitor* visitor);

#endif