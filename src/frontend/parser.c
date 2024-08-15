#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "memory.h"
#include "logger.h"
#include "debug.h"

static Expression* parse_expression(Parser* parser, Precedence precedence);
static void parse_statement(Parser* parser, Statement* stmt);
static void free_expression(Expression* expr);

static void free_statement(Statement* stmt) {
    if (stmt == NULL) return;

    switch (stmt->type) {
        case STMT_EXPRESSION:
        case STMT_RETURN:
        case STMT_PRINT:
            free_expression(stmt->value);
            break;
        case STMT_ASSIGN:
            free_expression(stmt->value);
            // Assuming name is not dynamically allocated
            break;
        // Add cases for other statement types as needed
        default:
            fprintf(stderr, "Unknown statement type in free_statement\n");
            break;
    }

    // If the statement itself was dynamically allocated, free it
    free(stmt);  // Uncomment if statements are individually allocated
}

static void free_expression(Expression* expr) {
    if (expr == NULL) return;

    switch (expr->type) {
        case EXPR_INFIX:
            free_expression((Expression*)expr->infix.left);
            free_expression((Expression*)expr->infix.right);
            break;
        case EXPR_PREFIX:
            free_expression((Expression*)expr->prefix.right);
            break;
        case EXPR_IF:
            free_expression((Expression*)expr->if_expr.condition);
            free_statement((Statement*)expr->if_expr.consequence);
            if (expr->if_expr.alternative) {
                free_statement((Statement*)expr->if_expr.alternative);
            }
            break;
        // Add cases for other expression types as needed
        case EXPR_INT:
        case EXPR_FLOAT:
        case EXPR_BOOL:
            // These types don't have nested allocations
            break;
        default:
            // Handle any unexpected types
            fprintf(stderr, "Unknown expression type in free_expression\n");
            break;
    }

    free(expr);
}

void de_init_program(Program* program) {
    if (program == NULL) return;

    for (u64 i = 0; i < program->statement_count; i++) {
        free_statement(&program->statements[i]);
    }

    free(program->statements);
    free(program);
}

void de_init_parser(Parser* parser) {
    free(parser->lexer->tokens);
    free(parser->lexer);
    parser->current = 0;
    parser->has_error = false;
    parser->panic_mode = false;
    free(parser);
}

static OperatorType get_operator(TokenType type) {
    switch (type) {
        case TOKEN_PLUS: return PARSE_OP_ADD;
        case TOKEN_STAR: return PARSE_OP_MULTIPLY;
        case TOKEN_SLASH: return PARSE_OP_DIVIDE;
        case TOKEN_MINUS: return PARSE_OP_MINUS;
        case TOKEN_EQUAL_EQUAL: return PARSE_OP_EQUALITY;
        case TOKEN_GREATER_EQUAL: return PARSE_OP_EQUAL_GREATER;
        case TOKEN_GREATER: return PARSE_OP_GREATER;
        case TOKEN_BANG_EQUAL: return PARSE_OP_NOT_EQUAL;
        case TOKEN_LESS: return PARSE_OP_LESS;
        case TOKEN_LESS_EQUAL: return PARSE_OP_EQUAL_LESS;
        default: return OP_UNKNOWN;
    }
}

static Precedence get_token_precedence(Token token) {
    switch (token.type) {
        default: {
            return LOWEST;
            break;
        }
        case TOKEN_ASSIGN: {
            return ASSIGN;
            break;
        }
        case TOKEN_OR: {
            return LOGICAL_OR;
            break;
        }
        case TOKEN_AND: {
            return LOGICAL_AND;
            break;
        }
        case TOKEN_EQUAL:
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG_EQUAL: {
            return EQUALS;
            break;
        }
        case TOKEN_LESS_EQUAL:
        case TOKEN_LESS:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_GREATER: {
            return LESSGREATER;
            break;
        }
        case TOKEN_PLUS:
        case TOKEN_MINUS: {
            return SUM;
            break;
        }
        case TOKEN_PERCENT:
        case TOKEN_SLASH:
        case TOKEN_STAR: {
            return PRODUCT;
            break;
        }
        case TOKEN_LEFT_PAREN: {
            return CALL;
            break;
        }
        case TOKEN_LEFT_BRACE: {
            return INDEX;
            break;
        }
    }
}

bool get_literal(Token* token, char* buffer, size_t buffer_size) {
    if (token == NULL || token->start == NULL || buffer == NULL) {
        return false;
    }
    if (token->length >= buffer_size) return false;
    memcpy(buffer, token->start, token->length);
    buffer[token->length] = '\0';
    return true;
}

static Expression* create_expression(ExpressionType type, Token token) {
    Expression* expr = ALLOCATE(Expression, 1);
    if (expr == NULL) {
        ERROR("Out of memory when allocating expressions");
        exit(EXIT_FAILURE);
    }
    expr->type = type;
    expr->token = token;
    return expr;
}

static void next_token(Parser* parser) {
    parser->current_token = parser->peek_token;
    if (parser->peek_token.type != TOKEN_EOF) {
        parser->peek_token = parser->lexer->tokens[parser->current];
    }
    parser->current++;
}

Parser* init_parser(Lexer* lexer) {
    Parser* parser = ALLOCATE(Parser, 1);
    if (!parser) {
        ERROR("Ran out of memory when allocating parser");
        exit(EXIT_FAILURE);
    }
    parser->has_error = false;
    parser->panic_mode = false;
    parser->current = 0;
    parser->lexer = lexer;
    next_token(parser);
    next_token(parser);
    return parser;
}

static void error(Parser* parser, const char* message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    Token* token = &parser->current_token;
    fprintf(stderr, "[line %lu] Error", token->line);
    if (token->type == TOKEN_EOF) fprintf(stderr, " at end.");
    if (token->type == TOKEN_ERROR) {

    } else {
        fprintf(stderr, " at '%.*s'", (int)token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
    parser->has_error = true;
}

static void peek_error(Parser* parser, TokenType type)  {
    const char* expected = print_token_type(type);
    const char* actual = print_token_type(parser->peek_token.type);
    ERROR("Line: [ %d ] -> Expected next token to be %s but got %s instead", parser->current_token.line, expected, actual);
    exit(EXIT_FAILURE);
}

static void add_statement(Program* program, Statement* statement) {
    if (program->statement_count == program->statement_capacity) {
        u64 old_capacity = program->statement_capacity;
        program->statement_capacity = GROW_CAPACITY(program->statement_capacity);
        program->statements = GROW_ARRAY(Statement, program->statements, old_capacity, program->statement_capacity);
        if (program->statements == NULL) {
            ERROR("Failed to allocate memory for statements in program.");
            exit(EXIT_FAILURE);
        }
    }

    program->statements[program->statement_count] = *statement;
    program->statement_count++;
}

static bool current_token_is(Parser* parser, TokenType type) {
    return parser->current_token.type == type;
}

static bool peek_token_is(Parser* parser, TokenType type) {
    return parser->peek_token.type == type;
}

static bool expect_peek(Parser* parser, TokenType type) {
    if (peek_token_is(parser, type)) {
        next_token(parser);
        return true;
    } else {
        peek_error(parser, type);
        return false;
    }
}

static void parse_assignment_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_ASSIGN;
    statement->token = parser->current_token;

    if (!expect_peek(parser, TOKEN_ASSIGN)) {
        return;
    }
    Identifier ident = {.token = parser->current_token};
    if (!get_literal(&statement->token, ident.value, sizeof(ident.value))) {
        ERROR("Unable to get ident value from string literal");
        exit(EXIT_FAILURE);
    }
    statement->name = ident;
    next_token(parser);
    statement->value = parse_expression(parser, LOWEST);

    while (!current_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
}

static void parse_return_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_RETURN;
    statement->token = parser->current_token;
    next_token(parser);
    while (!current_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
}

static Expression* parse_number_expression(Parser* parser) {
    char literal[MAX_TOKEN_LENGTH];
    if (!get_literal(&parser->current_token, literal, sizeof(literal))) {
        ERROR("Unable to get string literal for use in parsing number");
        exit(EXIT_FAILURE);
    }
    for (u64 i = 0; i < parser->current_token.length; i++) {
        if (literal[i] == '.') {
            char* eptr;
            Expression* expr = create_expression(EXPR_FLOAT, parser->current_token);
            expr->floating_point = (f64)strtod(literal, &eptr);
            return expr;
        }
    }
    Expression* expr = create_expression(EXPR_INT, parser->current_token);
    expr->integer = (i64)atoi(literal);
    return expr;
}

static Expression* parse_prefix_expression(Parser* parser) {
    Expression* expr = create_expression(EXPR_PREFIX, parser->current_token);
    expr->prefix.operator = get_operator(parser->current_token.type);
    next_token(parser);
    expr->prefix.right = (struct Expression*)parse_expression(parser, PREFIX);
    return expr;
}

static Expression* parse_infix_expression(Parser* parser, Expression* left) {
    Expression* expr = create_expression(EXPR_INFIX, parser->current_token);
    expr->infix.left = (struct Expression*)left;
    expr->infix.operator = get_operator(parser->current_token.type);
    Precedence precedence = get_token_precedence(parser->current_token);
    next_token(parser);
    Expression* right_expr = parse_expression(parser, precedence);
    expr->infix.right = (struct Expression*)right_expr;
    if (expr->infix.operator == PARSE_OP_DIVIDE && right_expr->integer == 0) {
        error(parser, "Divide by zero error!");
    }
    return expr;
}

static Expression* parse_boolean_expression(Parser* parser) {
    Expression* expr = create_expression(EXPR_BOOL, parser->current_token);
    expr->boolean = (parser->current_token.type == TOKEN_TRUE);
    return expr;
}

static Expression* parse_grouped_expression(Parser* parser) {
    next_token(parser);
    Expression* expr = parse_expression(parser, LOWEST);
    if (!expect_peek(parser, TOKEN_RIGHT_PAREN)) {
        return NULL;
    }
    return expr;
}

static Statement* parse_block_statement(Parser* parser) {
    u64 statement_count = 1;
    Statement* block_stmt = ALLOCATE(Statement, statement_count);
    next_token(parser);

    while (!current_token_is(parser, TOKEN_RIGHT_BRACE) && !current_token_is(parser, TOKEN_EOF)) {
        Statement* stmt = ALLOCATE(Statement, 1);
        parse_statement(parser, stmt);
        if (stmt != NULL) {
            block_stmt = GROW_ARRAY(Statement, block_stmt, statement_count, statement_count + 1);
            block_stmt[statement_count - 1] = *stmt;
            statement_count++;
        }
        if (peek_token_is(parser, TOKEN_DOT)) next_token(parser);
        next_token(parser);
    }
    return block_stmt;
}

static Expression* parse_if_expression(Parser* parser) {
    Expression* expr = create_expression(EXPR_IF, parser->current_token);

    if (!expect_peek(parser, TOKEN_LEFT_PAREN)) return NULL;

    next_token(parser);
    expr->if_expr.condition = (struct Expression*)parse_expression(parser, LOWEST);

    if (!expect_peek(parser, TOKEN_RIGHT_PAREN)) return NULL;

    expr->if_expr.consequence = (struct Statement*)parse_block_statement(parser);

    if (peek_token_is(parser, TOKEN_ELSE)) {
        next_token(parser);
        if (!expect_peek(parser, TOKEN_LEFT_BRACE)) {
            return NULL;
        }
        struct Statement* alt = (struct Statement*)parse_block_statement(parser);
        expr->if_expr.alternative = alt;
    }
    return expr;
}

static Expression* parse_expression(Parser* parser, Precedence precedence) {
    Expression* left = NULL;
    switch (parser->current_token.type) {
        case TOKEN_IF: {
            left = parse_if_expression(parser);
            break;
        }
        case TOKEN_LEFT_PAREN: {
            left = parse_grouped_expression(parser);
            break;
        }
        case TOKEN_TRUE:
        case TOKEN_FALSE: {
            left = parse_boolean_expression(parser);
            break;
        }
        case TOKEN_NUMBER: {
            left = parse_number_expression(parser);
            break;
        }
        case TOKEN_MINUS:
        case TOKEN_BANG: {
            left = parse_prefix_expression(parser);
            break;
        }
        default: {
            printf("CANT PARSE EXPR\n");
            break;
        }
    }

    while(!peek_token_is(parser, TOKEN_DOT) && precedence < get_token_precedence(parser->peek_token)) {
        switch (parser->peek_token.type) {
            case TOKEN_PLUS:
            case TOKEN_STAR:
            case TOKEN_SLASH:
            case TOKEN_GREATER:
            case TOKEN_GREATER_EQUAL:
            case TOKEN_EQUAL_EQUAL:
            case TOKEN_BANG_EQUAL:
            case TOKEN_LESS:
            case TOKEN_LESS_EQUAL:
            case TOKEN_MINUS: {
                next_token(parser);
                left = parse_infix_expression(parser, left);
                break;
            }
            default: break;
        }
    }
    return left;
}

static void parse_expression_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_EXPRESSION;
    statement->token = parser->current_token;
    statement->value = parse_expression(parser, LOWEST);
    if (!peek_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
}

static void parse_print_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_PRINT;
    statement->token = parser->current_token;
    next_token(parser);
    statement->value = parse_expression(parser, LOWEST);
    if (!peek_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
}

static void parse_statement(Parser* parser, Statement* stmt) {
    switch (parser->current_token.type) {
        case TOKEN_IDENTIFIER: {
            if (parser->peek_token.type == TOKEN_ASSIGN) {
                parse_assignment_statement(parser, stmt);
            }
            break;
        }
        case TOKEN_RETURN: {
            parse_return_statement(parser, stmt);
            break;
        }
        case TOKEN_PRINT: {
            parse_print_statement(parser, stmt);
            break;
        }
        default: {
            parse_expression_statement(parser, stmt);
            break;
        }
    }
}

static Program* init_program() {
    Program* program = ALLOCATE(Program, 1);
    if (!program) {
        ERROR("Out of memory, unable to parse program.");
        exit(EXIT_FAILURE);
    }
    program->statement_count = 0;
    program->statement_capacity = 1;
    program->statements = (Statement*)malloc(sizeof(Statement));
    return program;
}

Program* parse_program(Parser* parser) {
    Program* program = init_program();

     while (parser->current_token.type != TOKEN_EOF) {

         Statement* stmt = {};
         parse_statement(parser, stmt);
         if (stmt != NULL) {
            add_statement(program, stmt);
         }
         free(stmt);

         if (peek_token_is(parser, TOKEN_DOT)) next_token(parser);

         next_token(parser);
     }
    #ifdef DEBUG_MODE_PARSER
        printf("--- PROGRAM ---\n");
        debug_program(program);
        printf("--- PROGRAM ---\n\n");
    #endif
    return program;
}
