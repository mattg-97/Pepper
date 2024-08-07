#ifndef pepper_vm_h
#define pepper_vm_h

#define STACK_MAX 256

#include "common.h"
#include "chunk.h"
#include "value.h"
#include "debug.h"

typedef enum {
    OK,
} Result;

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stack_top;
} VM;

VM* init_vm(Chunk* chunk);
void free_vm(VM* vm);
Result run(VM* vm);

#endif
