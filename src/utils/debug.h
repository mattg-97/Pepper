#ifndef pepper_debug_h
#define pepper_debug_h

#include <common.h>
#include "lexer.h"
#include "parser.h"

void debug_token(Token* token);
void debug_statement(Statement* statement);
void debug_expression(Expression* expression);
void debug_program(Program* program);

#endif
