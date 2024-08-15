#ifndef pepper_vm_h
#define pepper_vm_h

#define STACK_MAX 256

#include "common.h"
#include "hashtable.h"
#include "chunk.h"
#include "bytecode_generator.h"


typedef enum {
    OK,
    RUNTIME_ERROR,
} Result;

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stack_top;
    void* objects;
    HashTable* strings;
    HashTable* globals;
} VM;

VM* init_vm(ByteCode* byte_code);
void free_vm(VM* vm);
Result run(VM* vm);
void add_chunk(VM* vm, Chunk* chunk);


#endif
