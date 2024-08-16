#include "bytecode_generator.h"
#include "lexer.h"
#include "memory.h"
#include "logger.h"
#include "parser.h"
#include "debug.h"
#include <stdint.h>

static void generate_expression(ByteCode* byte_code, Expression* expression);
static void generate_statement(ByteCode* byte_code, Statement* statement);

static void emit_byte(Chunk* chunk, uint8_t byte, u64 line) {
    write_chunk(chunk, byte, line);
}

static void emit_bytes(Chunk* chunk, uint8_t byte1, uint8_t byte2, u64 line) {
    emit_byte(chunk, byte1, line);
    emit_byte(chunk, byte2, line);
}

static uint8_t create_constant(Chunk* chunk, Value value) {
    const int constant = add_constant(chunk, value);
    if (constant > UINT8_MAX) {
        ERROR("Too many constants in one chunk");
        return 0;
    }
    return (uint8_t)constant;
}

static void emit_constant(Chunk* chunk, Value value, u64 line) {
    emit_bytes(chunk, OP_CONSTANT, create_constant(chunk, value), line);
}

static void generate_infix_expression(ByteCode* byte_code, Expression* expression) {
    generate_expression(byte_code, (Expression*)expression->infix.left);
    generate_expression(byte_code, (Expression*)expression->infix.right);
    const OperatorType operator = expression->infix.operator;
    switch (operator) {
        case PARSE_OP_ADD: emit_byte(byte_code->chunk, OP_ADD, expression->token.line); break;
        case PARSE_OP_MINUS: emit_byte(byte_code->chunk, OP_SUBTRACT, expression->token.line); break;
        case PARSE_OP_MULTIPLY: emit_byte(byte_code->chunk, OP_MULTIPLY, expression->token.line); break;
        case PARSE_OP_DIVIDE: emit_byte(byte_code->chunk, OP_DIVIDE, expression->token.line); break;
        case PARSE_OP_GREATER: emit_byte(byte_code->chunk, OP_GREATER, expression->token.line); break;
        default: emit_byte(byte_code->chunk, OP_UNKNOWN, expression->token.line); break;
    }
    //emit_byte(chunk, OP_POP, expression->token.line);
}

static void generate_int_expression(ByteCode* byte_code, Expression* expression) {
    emit_constant(byte_code->chunk, INT_VAL(expression->integer), expression->token.line);
}

static void generate_float_expression(ByteCode* byte_code, Expression* expression) {
    emit_constant(byte_code->chunk, FLOATING_VAL(expression->floating_point), expression->token.line);
}

static void generate_bool_expression(ByteCode* byte_code, Expression* expression) {
    emit_constant(byte_code->chunk, BOOL_VAL(expression->boolean), expression->token.line);
}

static void generate_if_expression(ByteCode* byte_code, Expression* expression) {
    generate_expression(byte_code, (Expression*)expression->if_expr.condition);
    generate_statement(byte_code, (Statement*)expression->if_expr.consequence);
    generate_statement(byte_code, (Statement*)expression->if_expr.alternative);
}

static void generate_ident_expression(ByteCode* byte_code, Expression* expression) {
    const uint8_t const_val = create_constant(byte_code->chunk, STRING_VAL(expression->ident.value));
    emit_bytes(byte_code->chunk, OP_GET_GLOBAL, const_val, expression->token.line);
}

static void generate_expression(ByteCode* byte_code, Expression* expression) {
    switch (expression->type) {
        case EXPR_INFIX: generate_infix_expression(byte_code, expression); break;
        case EXPR_INT: generate_int_expression(byte_code, expression); break;
        case EXPR_FLOAT: generate_float_expression(byte_code, expression); break;
        case EXPR_BOOL: generate_bool_expression(byte_code, expression); break;
        case EXPR_IF: generate_if_expression(byte_code, expression); break;
        case EXPR_IDENT: generate_ident_expression(byte_code, expression); break;
        default: break;
    }
}

static void generate_instantiate_statement(ByteCode* byte_code, Statement* statement) {
    generate_expression(byte_code, statement->value);
    const uint8_t const_val = create_constant(byte_code->chunk, STRING_VAL(statement->name.value));
    emit_bytes(byte_code->chunk, OP_DEFINE_GLOBAL, const_val, statement->token.line);
}

static void generate_assign_statement(ByteCode* byte_code, Statement* statement) {
    generate_expression(byte_code, statement->value);
    const uint8_t const_val = create_constant(byte_code->chunk, STRING_VAL(statement->name.value));
    emit_bytes(byte_code->chunk, OP_SET_GLOBAL, const_val, statement->token.line);
}

static void generate_statement(ByteCode* byte_code, Statement* statement) {
    switch (statement->type) {
        case STMT_EXPRESSION: {
            generate_expression(byte_code, statement->value);
            break;
        }
        case STMT_PRINT: {
            generate_expression(byte_code, statement->value);
            emit_byte(byte_code->chunk, OP_PRINT, statement->token.line);
            break;
        }
        case STMT_ASSIGN: {
            generate_assign_statement(byte_code, statement);
            break;
        }
        case STMT_INSTANTIATE: {
            generate_instantiate_statement(byte_code, statement);
            break;
        }
        default: break;
    }
}

static void init_bytecode(ByteCode* byte_code) {
    Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
    init_chunk(chunk);
    byte_code->chunk = chunk;
    byte_code->globals = hash_table_init();
    byte_code->strings = hash_table_init();
}

ByteCode* generate_bytecode(Program* program) {
    ByteCode* byte_code = ALLOCATE(ByteCode, 1);
    init_bytecode(byte_code);

    for (u64 i = 0; i < program->statement_count; i++) {
        generate_statement(byte_code, &program->statements[i]);
    }
    emit_byte(byte_code->chunk, OP_RETURN, program->statements[program->statement_count - 1].token.line);
    #ifdef DEBUG_MODE_INTERPRETER
    debug_chunk(byte_code->chunk);
    #endif
    return byte_code;
}

void free_byte_code(ByteCode* byte_code) {
    free_chunk(byte_code->chunk);
    hash_table_destroy(byte_code->globals);
    hash_table_destroy(byte_code->strings);
}