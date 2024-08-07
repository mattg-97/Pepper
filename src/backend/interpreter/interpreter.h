#ifndef pepper_interpreter_h
#define pepper_interpreter_h

#include "common.h"
#include "chunk.h"
#include "parser.h"

Chunk* interpret_program(Program* program);

#endif
