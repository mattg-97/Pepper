#include "bytecode_generator.h"
#include "chunk.h"
#include "lexer.h"
#include "memory.h"
#include "logger.h"
#include "parser.h"
#include "debug.h"
#include "value.h"
#include <stdint.h>

static void generate_expression(Chunk* chunk, Expression* expression);

static void emit_byte(Chunk* chunk, uint8_t byte, u64 line) {
    write_chunk(chunk, byte, line);
}

static void emit_bytes(Chunk* chunk, uint8_t byte1, uint8_t byte2, u64 line) {
    emit_byte(chunk, byte1, line);
    emit_byte(chunk, byte2, line);
}

static uint8_t create_constant(Chunk* chunk, Value value) {
    int constant = add_constant(chunk, value);
    if (constant > UINT8_MAX) {
        ERROR("Too many constants in one chunk");
        return 0;
    }
    return (uint8_t)constant;
}

static void emit_constant(Chunk* chunk, Value value, u64 line) {
    emit_bytes(chunk, OP_CONSTANT, create_constant(chunk, value), line);
}

static void generate_infix_expression(Chunk* chunk, Expression* expression) {
    generate_expression(chunk, (Expression*)expression->infix.left);
    generate_expression(chunk, (Expression*)expression->infix.right);
    OperatorType operator = expression->infix.operator;
    switch (operator) {
        case PARSE_OP_ADD: emit_byte(chunk, OP_ADD, expression->token.line); break;
        case PARSE_OP_MINUS: emit_byte(chunk, OP_SUBTRACT, expression->token.line); break;
        case PARSE_OP_MULTIPLY: emit_byte(chunk, OP_MULTIPLY, expression->token.line); break;
        case PARSE_OP_DIVIDE: emit_byte(chunk, OP_DIVIDE, expression->token.line); break;
        default: emit_byte(chunk, OP_UNKNOWN, expression->token.line); break;
    }
    //emit_byte(chunk, OP_POP, expression->token.line);
}

static void generate_int_expression(Chunk* chunk, Expression* expression) {
    emit_constant(chunk, INT_VAL(expression->integer), expression->token.line);
}

static void generate_float_expression(Chunk* chunk, Expression* expression) {
    emit_constant(chunk, FLOATING_VAL(expression->floating_point), expression->token.line);
}

static void generate_expression(Chunk* chunk, Expression* expression) {
    switch (expression->type) {
        case EXPR_INFIX: generate_infix_expression(chunk, expression); break;
        case EXPR_INT: generate_int_expression(chunk, expression); break;
        case EXPR_FLOAT: generate_float_expression(chunk, expression); break;
        default: break;
    }
}

static void generate_statement(Chunk* chunk, Statement* statement) {
    switch (statement->type) {
        case STMT_EXPRESSION: {
            generate_expression(chunk, statement->value);
            break;
        }
        default: break;
    }
}

static void init_bytecode_generator(BytecodeGenerator* generator, Chunk* chunk) {
    generator->chunk = chunk;
}

Chunk* generate_bytecode(Program* program) {
    BytecodeGenerator generator;
    Chunk* chunk = ALLOCATE(Chunk, 1);
    init_chunk(chunk);
    init_bytecode_generator(&generator, chunk);

    for (u64 i = 0; i < program->statement_count; i++) {
        generate_statement(chunk, &program->statements[i]);
    }
    emit_byte(chunk, OP_RETURN, program->statements[program->statement_count - 1].token.line);
    #ifdef DEBUG_MODE_INTERPRETER
    debug_chunk(chunk);
    #endif
    return chunk;
}
