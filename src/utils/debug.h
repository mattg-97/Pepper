#ifndef pepper_debug_h
#define pepper_debug_h

#include <common.h>
#include "lexer.h"
#include "ast.h"

void debug_token(Token* token);
void debug_binary_expression(Visitor* visitor, Binary* binary);

#endif