#ifndef pepper_debug_h
#define pepper_debug_h

#include <common.h>
#include "../frontend/lexer.h"

const char* print_token_type(TokenType type);
char* print_token_literal(Token* token);

#endif