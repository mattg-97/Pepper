#include "parser.h"
#include "logger.h"
#include "debug.h"

Parser parser;

Parser* init_parser(Token* tokens) {
    parser.current = 0;
    parser.tokens = tokens;
    next_token();
    next_token();
    return &parser;
}

void next_token() {
    parser.currentToken = parser.peekToken;
    parser.peekToken = parser.tokens[parser.current];
    parser.current++;
}

static void add_statement(Program* program, Statement statement) {
    if (program->statementCount == program->statementCapacity) {
        program->statementCapacity = program->statementCapacity == 0 ? 8 : program->statementCapacity * 2;
        program->statements = realloc(program->statements, program->statementCapacity * sizeof(Statement));
        if (program->statements == NULL) {
            ERROR("Failed to allocate memory for statements in program.");
            exit(EXIT_FAILURE);
        }
    }
    program->statements[program->statementCount] = statement;
    program->statementCount++;
}

static bool current_token_is(TokenType type) {
    return parser.currentToken.type == type;
}

static bool peek_token_is(TokenType type) {
    return parser.peekToken.type == type;
}

static bool expect_peek(TokenType type) {
    if (peek_token_is(type)) {
        next_token();
        return true;
    } else {
        return false;
    }
}

/*static AssignmentStatement* parse_assignment() {
    AssignmentStatement* assignment = (AssignmentStatement*)malloc(sizeof(AssignmentStatement));

    if (!expect_peek(TOKEN_ASSIGN)) return NULL;

    assignment->name->token = parser.currentToken;
    assignment->name->value = parser.currentToken.literal;

    do {
        next_token();
    } while (!current_token_is(TOKEN_DOT));

    return assignment;
}

static AssignmentStatement* parse_statement() {
    Statement* statement = (Statement*)malloc(sizeof(Statement));
    switch (parser.currentToken.type) {
        case TOKEN_IDENTIFIER: {
            if (parser.peekToken.type == TOKEN_ASSIGN) {
                return parse_assignment();
            }
            break;
        }
        default: {
            return NULL;
            break;
        }
    }
    return NULL;
} */

Program* parse_program() {
    Program* program = (Program*)malloc(sizeof(Program));
    program->statements = NULL;

    do {
        Statement* statement = parse_statement();
        add_statement(program, *statement);
        next_token();
    } while (parser.currentToken.type != TOKEN_EOF);

    return program;
}

Binary* parse_binary() {
    ConcreteVisitor* visitor = init_visitor();

    int a_val = atoi(parser.currentToken.literal);
    next_token();
    int b_val = atoi(parser.peekToken.literal);
    Literal a = init_literal(a_val);
    Literal b = init_literal(b_val);
    Binary bin = init_binary((Expression*)&a, TOKEN_PLUS, (Expression*)&b);

    bin.base.accept((Expression*)&bin, (Visitor*)visitor);
    debug_binary_expression(&visitor->base, &bin);
    return &bin;
}
