#ifndef pepper_bytecode_gen_h
#define pepper_bytecode_gen_h

#include "common.h"
#include "chunk.h"
#include "parser.h"
#include "hashtable.h"

typedef struct {
    Chunk* chunk;
    HashTable* globals;
    HashTable* strings;
} ByteCode;

ByteCode* generate_bytecode(Program* program);
void free_byte_code(ByteCode* byte_code);

#endif
