#include "parser.h"
#include "lexer.h"
#include "logger.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

/*static void print_help(Parser* parser) {
    printf("Current token: \n");
    debug_token(&parser->current_token);
    printf("Next token: \n");
    debug_token(&parser->peek_token);
}*/

static void next_token(Parser* parser) {
    parser->current_token = parser->peek_token;
    parser->peek_token = parser->tokens[parser->current];
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

static void print_assignment_statement(Statement* statement) {
    printf("YEET\n");
    printf("%s := %s", statement->token.literal, statement->name.value);
}

static void print_statement(Statement* statement) {
    switch (statement->type) {
        case STMT_ASSIGN: {
            print_assignment_statement(statement);
        }
        default: printf("CANT PRINT STATEMENT\n");
    }
}

static void add_statement(Program* program, Statement statement) {
    if (program->statement_count == program->statement_capacity) {
        program->statement_capacity = program->statement_capacity == 0 ? 8 : program->statement_capacity * 2;
        program->statements = realloc(program->statements, program->statement_capacity * sizeof(Statement));
        if (program->statements == NULL) {
            ERROR("Failed to allocate memory for statements in program.");
            exit(EXIT_FAILURE);
        }
    }
    program->statements[program->statement_count] = statement;
    print_statement(&statement);
    program->statement_count++;
}

/*static bool current_token_is(Parser* parser, TokenType type) {
    return parser->current_token.type == type;
    }*/

static bool peek_token_is(Parser* parser, TokenType type) {
    return parser->peek_token.type == type;
}

/*static bool expect_peek(Parser* parser, TokenType type) {
    if (peek_token_is(parser, type)) {
        next_token(parser);
        return true;
    } else {
        return false;
    }
    }*/

static Expression* make_expression(ExpressionType type, Token token) {
    Expression* expr = malloc(sizeof(Expression));
    if (!expr) {
        ERROR("Out of memory");
        exit(EXIT_FAILURE);
    }
    expr->type = type;
    expr->token = token;
    return expr;
}

static Expression* parse_int_expression(Parser* parser) {
    //printf("Parsing int: \n");
    //print_help(parser);
    Expression* expr = make_expression(EXPR_INT, parser->current_token);
    expr->integer = 0;
    char *s = expr->token.literal;
    expr->integer = atoi(s);
    return expr;
}

static Expression* parse_expression(Parser* parser);

static Expression* parse_infix_expression(Parser* parser, Expression* left) {
    //printf("Parsing infix: \n");
    //print_help(parser);
    Expression* expr = make_expression(EXPR_INFIX, parser->current_token);
    expr->infix.left = (struct Expression *)left;
    expr->infix.operator = OP_ADD;
    next_token(parser);
    expr->infix.right = (struct Expression *)parse_expression(parser);
    return expr;
}

static Expression* parse_expression(Parser* parser) {
    //printf("Parsing expression: \n");
    //print_help(parser);
    Expression* left;
    switch (parser->current_token.type) {
        case TOKEN_NUMBER: {
            left = parse_int_expression(parser);
            break;
        }
        default: {
            //debug_token(&parser->peek_token);
            ERROR("FAILURE TO parse expression");
            break;
        }
    }
    while (!peek_token_is(parser, TOKEN_DOT)) {
        TokenType type = parser->peek_token.type;
        switch (type) {
            case TOKEN_PLUS: {
                next_token(parser);
                //printf("Parsing expression after next token call: \n");
                //print_help(parser);
                left = parse_infix_expression(parser, left);
                break;
            }
            default: {
                return left;
                break;
            }
        }
    }
    return left;
}

static int parse_assignment_statement(Parser* parser, Statement* statement) {
    //printf("Parsing assignment: \n");
    //print_help(parser);
    statement->type = STMT_ASSIGN;
    statement->token = parser->current_token;
    next_token(parser);
    //printf("Parsing assignment after next token call: \n");
    //print_help(parser);
    statement->value = parse_expression(parser);
    return 1;
}

static int parse_statement(Parser* parser, Statement* statement) {
    //printf("Parsing statement: \n");
    //print_help(parser);
    switch (parser->current_token.type) {
        case TOKEN_IDENTIFIER: {
            if (parser->peek_token.type == TOKEN_ASSIGN) {
                next_token(parser);
                //printf("Parsing statement after next token call: \n");
                //print_help(parser);
                return parse_assignment_statement(parser, statement);
            }
            break;
        }
        default: {
            return 0;
            break;
        }
    }
    INFO("%s", statement->name);
    return 0;
}

static void print_program(Program* program) {
    for (u64 i = 0; i < program->statement_count; i++) {
        printf("%s\n", program->statements[i].name.value);
    }
}

Program* parse_program(Parser* parser) {
    //printf("Parsing program: \n");
    //print_help(parser);
    const int capacity = 4;
    Program* program = (Program*)malloc((sizeof(Program) + capacity) * sizeof *program->statements);
    if (!program) {
        ERROR("Out of memory, unable to parse program.");
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG_MODE_PARSER
    #endif
     program->statement_count = 0;
     program->statement_capacity = 0;
     program->statements = (Statement*)malloc(sizeof(Statement));

     while (parser->current_token.type != TOKEN_EOF) {
         Statement stmt;
         if (parse_statement(parser, &stmt) == -1) {
            printf("Adding statement\n");
            add_statement(program, stmt);
            continue;
         }
         //program->statements[program->statement_count++] = stmt;
         next_token(parser);
     }
     print_program(program);
    return program;
}
