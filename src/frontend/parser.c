#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "memory.h"
#include "logger.h"
#include "debug.h"

/*static void print_help(Parser* parser) {
    printf("Current token: \n");
    debug_token(&parser->current_token);
    printf("Next token: \n");
    debug_token(&parser->peek_token);
}*/

static void next_token(Parser* parser) {
    parser->current_token = parser->peek_token;
    if (parser->tokens[parser->current].type != TOKEN_EOF) {
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


static void peek_error(Parser* parser, TokenType type)  {
    const char* expected = print_token_type(type);
    const char* actual = print_token_type(parser->peek_token.type);
    ERROR("Expected next token to be %s but got %s instead", expected, actual);
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

static int parse_assignment_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_ASSIGN;
    statement->token = parser->current_token;

    if (!expect_peek(parser, TOKEN_ASSIGN)) {
        return -1;
    }
    Identifier ident;
    ident.token = parser->current_token;
    memcpy(&ident.value, &parser->current_token.literal, parser->current_token.length);
    statement->name = ident;

    while (!current_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
    return 1;
}

static int parse_return_statement(Parser* parser, Statement* statement) {
    statement->type = STMT_RETURN;
    next_token(parser);
    while (!current_token_is(parser, TOKEN_DOT)) {
        next_token(parser);
    }
    return 1;
}

static int parse_statement(Parser* parser, Statement* statement) {
    switch (parser->current_token.type) {
        case TOKEN_IDENTIFIER: {
            if (parser->peek_token.type == TOKEN_ASSIGN) {
                return parse_assignment_statement(parser, statement);
            } else {
                return -1;
            }
            break;
        }
        case TOKEN_RETURN: {
            return parse_return_statement(parser, statement);
            break;
        }
        default: {
            return -1;
            break;
        }
    }
    return -1;
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
         Statement stmt;
         if (parse_statement(parser, &stmt) != -1) {
            add_statement(program, &stmt);
         };
         next_token(parser);
     }
     for (u64 i = 0; i < program->statement_count; i++) {
        debug_statement(&program->statements[i]);
     }
    return program;
}
