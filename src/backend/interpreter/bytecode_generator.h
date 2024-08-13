#ifndef pepper_interpreter_h
#define pepper_interpreter_h

#include "common.h"
#include "chunk.h"
#include "parser.h"

typedef struct {
    Chunk* chunk;
} BytecodeGenerator;

Chunk* generate_bytecode(Program* program);

#endif
