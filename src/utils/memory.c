#include <string.h>
#include "memory.h"

void* set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void* copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

void* zero_memory(void* block, u64 size) {
    return memset(block, 0, size);
}

void free_memory(void* block, bool aligned) {
    aligned ? free(block) : free(block);
}

void* allocate_memory(u64 size, bool aligned) {
    if (aligned) {
        return malloc(size);
    } else {
        return malloc(size);
    }
}