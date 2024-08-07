#include "chunk.h"
#include "memory.h"
#include "value.h"

void init_chunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    init_value_array(&chunk->constants);
}

void write_chunk(Chunk* chunk, uint8_t byte, u64 line) {
    if (chunk->capacity < chunk->count + 1) {
        u64 old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, old_capacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(size_t, chunk->lines, old_capacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

void free_chunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(size_t, chunk->lines, chunk->capacity);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

int add_constant(Chunk* chunk, Value value) {
    write_value_array(&chunk->constants, value);
    return (int)(chunk->constants.count - 1);
}

void write_constant(Chunk* chunk, Value value, u64 line) {
    int constant_index = add_constant(chunk, value);
    if (chunk->capacity < chunk->count + 4) {
        u64 old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, old_capacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(size_t, chunk->lines, old_capacity, chunk->capacity);
    }
    chunk->code[chunk->count] = (constant_index >> 16) & 0xFF;
    chunk->lines[chunk->count] = line;
    chunk->count++;

    chunk->code[chunk->count] = (constant_index >> 8) & 0xFF;
    chunk->lines[chunk->count] = line;
    chunk->count++;

    chunk->code[chunk->count] = constant_index & 0xFF;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}
