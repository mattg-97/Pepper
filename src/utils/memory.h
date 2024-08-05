#ifndef pepper_memory_h
#define pepper_memory_h

#include "common.h"

void* set_memory(void* dest, i32 value, u64 size);
void* copy_memory(void* dest, const void* source, u64 size);
void* zero_memory(void* block, u64 size);
void free_memory(void* block, bool aligned);
void * allocate_memory(u64 size, bool aligned);

#endif
