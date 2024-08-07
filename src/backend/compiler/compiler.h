#ifndef pepper_compiler_h
#define pepper_compiler_h

#include "vm.h"

typedef struct {
    void* constants;
} Compiler;

Compiler* init_compiler();

void compile(const char* source);

#endif
