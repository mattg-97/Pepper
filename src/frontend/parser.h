#ifndef pepper_parser_h
#define pepper_parser_h

#include "lexer.h"

typedef enum {
    LOWEST = 0,
    ASSIGN,
    LOGICAL_OR,
    LOGICAL_AND,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL,
    INDEX,
} Precedence;

typedef enum {
    EXPR_INFIX = 1,
    EXPR_PREFIX,
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_BOOL,
    EXPR_IF,
} ExpressionType;

typedef enum {
    STMT_ASSIGN = 1,
    STMT_RETURN,
    STMT_EXPRESSION,
    STMT_PRINT,
} StatementType;

typedef enum {
    PARSE_OP_ADD = 1,
    PARSE_OP_MINUS,
    PARSE_OP_DIVIDE,
    PARSE_OP_MULTIPLY,
    PARSE_OP_EQUALITY,
    PARSE_OP_GREATER,
    PARSE_OP_EQUAL_GREATER,
    PARSE_OP_LESS,
    PARSE_OP_EQUAL_LESS,
    PARSE_OP_NOT_EQUAL,
    OP_UNKNOWN,
} OperatorType;

typedef struct {
    Lexer* lexer;
    Token current_token;
    Token peek_token;
    u64 current;
    bool has_error;
    bool panic_mode;
} Parser;

struct Expression;
struct Statement;

typedef struct {
    struct Expression* left;
    struct Expression* right;
    OperatorType operator;
} InfixExpression;

typedef struct {
    struct Expression* right;
    OperatorType operator;
} PrefixExpression;

typedef struct {
    struct Expression* condition;
    struct Statement* consequence;
    struct Statement* alternative;
} IfExpression;

typedef struct {
    ExpressionType type;
    Token token;
    union {
        i64 integer;
        f64 floating_point;
        bool boolean;
        InfixExpression infix;
        PrefixExpression prefix;
        IfExpression if_expr;
    };
} Expression;

typedef struct {
    char value[MAX_TOKEN_LENGTH];
    Token token;
} Identifier;

typedef struct {
    StatementType type;
    Token token;
    Identifier name;
    Expression *value;
} Statement;

typedef struct {
    Token token;
    Expression return_value;
} ReturnStatement;

typedef struct {
    Token token;
    union {
        i64 integer;
        f64 floating_point;
    };
} NumberExpression;

typedef struct {
    Token token;
    Expression expression;
} ExpressionStatement;

typedef struct {
    Statement* statements;
    u64 statement_count;
    u64 statement_capacity;
} Program;

Parser* init_parser(Lexer* lexer);
void de_init_parser(Parser* parser);
void de_init_program(Program* program);
Program* parse_program(Parser* parser);
bool get_literal(Token* token, char* buffer, size_t buffer_size);

#endif
