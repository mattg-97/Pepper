#ifndef pepper_bytecode_gen_h
#define pepper_bytecode_gen_h

#include "common.h"
#include "chunk.h"
#include "parser.h"

typedef struct {
    Chunk* chunk;
} BytecodeGenerator;

Chunk* generate_bytecode(Program* program);

#endif
