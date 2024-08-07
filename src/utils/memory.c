#include "memory.h"

void* reallocate(void* pointer, u64 oldSize, u64 newSize) {
    if (newSize == 0 && oldSize <= 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL) return NULL;
    return result;
}
