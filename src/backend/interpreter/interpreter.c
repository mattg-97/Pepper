#include "interpreter.h"
#include "memory.h"
#include "logger.h"
#include "parser.h"
#include "debug.h"
#include <stdint.h>


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

static void interpret_statement(Chunk* chunk, Statement* statement) {
    switch (statement->type) {
        case STMT_EXPRESSION: {
            switch (statement->value->type) {
                case EXPR_INFIX: {
                    Expression* left = (Expression*)statement->value->infix.left;
                    Expression* right = (Expression*)statement->value->infix.right;
                    emit_constant(chunk, INT_VAL(left->integer), left->token.line);
                    emit_constant(chunk, INT_VAL(right->integer), right->token.line);
                    emit_byte(chunk, OP_ADD, right->token.line);
                    emit_byte(chunk, OP_POP, right->token.line);
                    break;
                }
                default: break;
            }
        }
        default: break;
    }
}

Chunk* interpret_program(Program* program) {
    Chunk* chunk = ALLOCATE(Chunk, 1);
    init_chunk(chunk);
    for (u64 i = 0; i < program->statement_count; i++) {
        interpret_statement(chunk, &program->statements[i]);
    }
    #ifdef DEBUG_MODE_INTERPRETER
    debug_chunk(chunk);
    #endif
    emit_byte(chunk, OP_RETURN, program->statements[program->statement_count].token.line);
    return chunk;
}
