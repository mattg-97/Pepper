#ifndef pepper_debug_h
#define pepper_debug_h

#include <common.h>
#include "lexer.h"
#include "parser.h"
#include "chunk.h"

void debug_token(Token* token);
void debug_statement(Statement* statement);
void debug_expression(Expression* expression);
void debug_program(Program* program);
const char* print_token_type(TokenType type);
void debug_chunk(Chunk* chunk);
int disassemble_instruction(Chunk* chunk, int offset);

#endif
