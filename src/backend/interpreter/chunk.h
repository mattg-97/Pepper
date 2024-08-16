#ifndef pepper_chunk_h
#define pepper_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_RETURN,
    OP_POP,
    OP_NEGATE,
    OP_PRINT,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GREATER,
} OpCode;

typedef struct {
    u64 count;
    u64 capacity;
    uint8_t* code;
    size_t* lines;
    ValueArray constants;
} Chunk;

void init_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8_t byte, u64 line);
void free_chunk(Chunk* chunk);
int add_constant(Chunk* chunk, Value value);
void write_constant(Chunk* chunk, Value value, u64 line);

#endif
