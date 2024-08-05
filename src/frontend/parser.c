#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "logger.h"
#include "debug.h"

static void parse_expression(Parser* parser);

/*static void print_help(Parser* parser) {
    printf("Current token: \n");
    debug_token(&parser->current_token);
    printf("Next token: \n");
    debug_token(&parser->peek_token);
}*/

static void next_token(Parser* parser) {
    parser->current_token = parser->peek_token;
    if (parser->peek_token.type != TOKEN_EOF) {
        parser->peek_token = parser->tokens[parser->current];
    }
    parser->current++;
}

Parser* init_parser(Token* tokens) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) {
        ERROR("Ran out of memory when allocating parser");
        exit(EXIT_FAILURE);
    }
    parser->current = 0;
    parser->tokens = tokens;
    next_token(parser);
    next_token(parser);
    return parser;
}

void de_init_parser(Parser* parser) {
    parser->current = 0;
    free(parser->tokens);
}

static void peek_error(Parser* parser, TokenType type)  {
    const char* expected = print_token_type(type);
    const char* actual = print_token_type(parser->peek_token.type);
    ERROR("Line: [ %d ] -> Expected next token to be %s but got %s instead", parser->current_token.line, expected, actual);
    exit(EXIT_FAILURE);
}

static void add_statement(Program* program, Statement* statement) {
    if (program->statement_count == program->statement_capacity) {
        program->statement_capacity = program->statement_capacity == 0 ? 8 : program->statement_capacity * 2;
        program->statements = realloc(program->statements, program->statement_capacity * sizeof(Statement));
        if (program->statements == NULL) {
            ERROR("Failed to allocate memory for statements in program.");
            exit(EXIT_FAILURE);
        }
    }
    debug_statement(statement);
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

/*static Expression* make_expression(ExpressionType type, Token token) {
    Expression* expr = malloc(sizeof(Expression));
    if (!expr) {
        ERROR("Out of memory");
        exit(EXIT_FAILURE);
    }
    expr->type = type;
    expr->token = token;
    return expr;
}*/

static void parse_assignment_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_ASSIGN;
    statement->token = parser->current_token;

    if (!expect_peek(parser, TOKEN_ASSIGN)) {
        return;
    }
    Identifier ident = {.token = parser->current_token};
    strcpy(ident.value, parser->current_token.literal);
    statement->name = ident;
    Expression* expression = (Expression*)malloc(sizeof(Expression));
    expression->type = EXPR_INT;
    expression->token = parser->peek_token;
    statement->value = expression;
    statement->value->token = parser->peek_token;

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

static void parse_number_literal(Parser* parser, NumberLiteral* lit) {
    printf("Parsing intlit: token -> %s\n", parser->current_token.literal);
    lit->token = parser->current_token;
    for (u64 i = 0; i < parser->current_token.length; i++) {
        if (parser->current_token.literal[i] == '.') {
            char* eptr;
            lit->floating_point = (f64)strtod(parser->current_token.literal, &eptr);
            return;
        }
    }
    lit->integer = (i64)atoi(parser->current_token.literal);
}

static void parse_expression(Parser* parser) {
    printf("%s\n", parser->peek_token.literal);
    switch (parser->peek_token.type) {
        case TOKEN_NUMBER: {
            NumberLiteral* literal = (NumberLiteral*)malloc(sizeof(NumberLiteral));
            parse_number_literal(parser, literal);
        }
        default: {
            printf("CANT PARSE EXPR\n");
        }
    }
}

static void parse_expression_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_EXPRESSION;
    statement->token = parser->current_token;
    parse_expression(parser);
    if (!peek_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
}

static Statement* parse_statement(Parser* parser) {
    Statement* stmt = (Statement*)malloc(sizeof(Statement));
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
        default: {
            parse_expression_statement(parser, stmt);
            break;
        }
    }
    return stmt;
}

static Program* init_program() {
    Program* program = (Program*)malloc(sizeof(Program));
    if (!program) {
        ERROR("Out of memory, unable to parse program.");
        exit(EXIT_FAILURE);
    }
    program->statement_count = 0;
    program->statement_capacity = 0;
    program->statements = (Statement*)malloc(sizeof(Statement));
    return program;
}

Program* parse_program(Parser* parser) {
    Program* program = init_program();

    #ifdef DEBUG_MODE_PARSER
    #endif
     while (parser->current_token.type != TOKEN_EOF) {
         Statement* stmt = parse_statement(parser);
         if (stmt != NULL) {
            add_statement(program, stmt);
         };
         next_token(parser);
     }
     /*for (u64 i = 0; i < program->statement_count; i++) {
        debug_statement(&program->statements[i]);
     }*/
    return program;
}
