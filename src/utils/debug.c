#include "debug.h"
#include "logger.h"
#include "parser.h"
#include "lexer.h"

const char* print_token_type(TokenType type) {
    switch (type) {
        // Single character tokens.
        case TOKEN_LEFT_BRACE: return "TOKEN_LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "TOKEN_RIGHT_BRACE";
        case TOKEN_RIGHT_PAREN: return "TOKEN_RIGHT_PAREN";
        case TOKEN_LEFT_PAREN: return "TOKEN_LEFT_PAREN";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_DOT: return "TOKEN_DOT";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_SLASH: return "TOKEN_SLASH";
        case TOKEN_STAR: return "TOKEN_STAR";
        case TOKEN_QUESTION_MARK: return "TOKEN_QUESTION_MARK";
        case TOKEN_AT: return "TOKEN_AT";
        case TOKEN_PERCENT: return "TOKEN_PERCENT";
        // One or two character tokens
        case TOKEN_BANG: return "TOKEN_BANG";
        case TOKEN_BANG_EQUAL: return "TOKEN_BANG_EQUAL";
        case TOKEN_EQUAL: return "TOKEN_EQUAL";
        case TOKEN_EQUAL_EQUAL: return "TOKEN_EQUAL_EQUAL";
        case TOKEN_GREATER: return "TOKEN_GREATER";
        case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";
        case TOKEN_LESS: return "TOKEN_LESS";
        case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_COLON_COLON: return "TOKEN_COLON_COLON";
        case TOKEN_DECREMENT: return "TOKEN_DECREMENT";
        case TOKEN_INCREMENT: return "TOKEN_INCREMENT";
        // Literals
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_NUMBER: return "TOKEN_NUMBER";
        // Keywords
        case TOKEN_AND: return "TOKEN_AND";
        case TOKEN_STRUCT: return "TOKEN_STRUCT";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_FALSE: return "TOKEN_FALSE";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_FN: return "TOKEN_FN";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_NIL: return "TOKEN_NIL";
        case TOKEN_OR: return "TOKEN_OR";
        case TOKEN_PRINT: return "TOKEN_PRINT";
        case TOKEN_RETURN: return "TOKEN_RETURN";
        case TOKEN_TRUE: return "TOKEN_TRUE";
        case TOKEN_LET: return "TOKEN_LET";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_METHOD: return "TOKEN_METHOD";
        case TOKEN_ENUM: return "TOKEN_ENUM";
        case TOKEN_CONST: return "TOKEN_CONST";
        case TOKEN_AS: return "TOKEN_AS";
        case TOKEN_MUT: return "TOKEN_MUT";
        case TOKEN_VOID: return "TOKEN_VOID";

        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_ERROR: return "TOKEN_ERROR";
    }
    return "TOKEN_UNKNOWN";
}

static const char* print_statement_type(StatementType type) {
    switch (type) {
        case STMT_ASSIGN: return "STMT_ASSIGN";
        case STMT_RETURN: return "STMT_RETURN";
        case STMT_EXPRESSION: return "STMT_EXPRESSION";
    }
    return "UNKNOWN_STATEMENT";
}

static const char* print_expression_type(ExpressionType type) {
    switch (type) {
        case EXPR_INFIX: return "Infix";
        default: return "UNKNOWN_EXPR";
    }
}

static char print_operator(OperatorType type) {
    switch (type) {
        case OP_ADD: return '+';
        default: return '.';
    }
}

void debug_token(Token* token) {
    const char* type = print_token_type(token->type);
    char literal[MAX_TOKEN_LENGTH];
    if (!get_literal(token, literal, sizeof(literal))) {
        ERROR("Unable to get string literal for use in debugging tokens");
        exit(EXIT_FAILURE);
    }
    printf("Token: |%s|  Type: |%s|\n", literal, type);
}

void debug_expression(Expression* expression) {
    switch (expression->type) {
        case EXPR_INT: {
            char literal[MAX_TOKEN_LENGTH];
            if (!get_literal(&expression->token, literal, sizeof(literal))) {
                ERROR("Unable to get string literal for use in debugging expressions");
                exit(EXIT_FAILURE);
            }
            printf("%s", literal);
            break;
        }
        case EXPR_INFIX: {
            printf("Expression: {\n\t\t\t");
            printf("Type: %s,\n\t\t\t", print_expression_type(expression->type));
            printf("Left: ");
            debug_expression((Expression*)expression->infix.left);
            printf(",\n\t\t\t");
            printf("Operator: %c,\n\t\t\t", print_operator(expression->infix.operator));
            printf("Right: ");
            debug_expression((Expression*)expression->infix.right);
            printf(",\n\t\t");
            printf("},\n");
            break;
        }
        default: {
            printf("YEEET");
            break;
        }
    }
}

void debug_statement(Statement* statement) {
    printf("Statement: {\n\t\t");
    printf("Type: %s,\n\t\t", print_statement_type(statement->type));
    switch (statement->type) {
        case STMT_ASSIGN: {
            char literal[MAX_TOKEN_LENGTH];
            if (!get_literal(&statement->token, literal, sizeof(literal))) {
                ERROR("Unable to get string literal for use in debugging statements");
                exit(EXIT_FAILURE);
            }
            printf("Ident: %s\n\t\t", statement->name.value);
            printf("Expr: ");
            debug_expression(statement->value);
            printf(".");
            printf("\n");
            break;
        }
        case STMT_RETURN: {
            printf("RETURN\n");
            printf("return statement.\n");
            break;
        }
        case STMT_EXPRESSION: {
            debug_expression(statement->value);
            break;
        }
        default: {
            printf("UNKNOWN\n");
            break;
        }
    }
    printf("\t},\n");
}

void debug_program(Program* program) {
    printf("Program: {\n");
    for (u64 i = 0; i < program->statement_count; i++) {
        printf("\t");
        debug_statement(&program->statements[i]);
    }
    printf("}\n");
}
