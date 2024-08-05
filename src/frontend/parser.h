#ifndef pepper_parser_h
#define pepper_parser_h

#include "lexer.h"

#define MAX_IDENT_LENGTH 128

typedef enum {
    EXPR_INFIX = 1,
    EXPR_INT,
} ExpressionType;

enum StatementType {
    STMT_ASSIGN = 1,
    STMT_RETURN,
};

typedef enum {
    OP_ADD = 1,
} OperatorType;

typedef struct {
    Token* tokens;
    Token current_token;
    Token peek_token;
    u64 current;
} Parser;

struct Expression;

typedef struct {
    struct Expression* left;
    struct Expression* right;
    OperatorType operator;
} InfixExpression;

typedef struct {
    ExpressionType type;
    Token token;
    union {
        int integer;
        InfixExpression infix;
    };
} Expression;

typedef struct {
    char value[MAX_IDENT_LENGTH];
    Token token;
} Identifier;

typedef struct {
    enum StatementType type;
    Token token;
    Identifier name;
    Expression *value;
} Statement;

typedef struct {
    Token token;
    Expression return_value;
} ReturnStatement;

typedef struct {
    Statement* statements;
    u64 statement_count;
    u64 statement_capacity;
} Program;

Parser* init_parser(Token* tokens);
void de_init_parser();
Program* parse_program(Parser* parser);

#endif
