#include "value.h"
#include "memory.h"

bool values_equal(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_INT: return AS_INT(a) == AS_INT(b);
        case VAL_FLOATING: return AS_FLOATING(a) == AS_FLOATING(b);
        case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
        default: return false; // Unreachable.
    }
}

void init_value_array(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}
void write_value_array(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
            u64 old_capacity = array->capacity;
            array->capacity = (u64)GROW_CAPACITY(old_capacity);
            array->values = GROW_ARRAY(Value, array->values, old_capacity, array->capacity);
        }
        array->values[array->count] = value;
        array->count++;
}

void free_value_array(ValueArray* array) {
    FREE_ARRAY(Value, array, array->capacity);
    init_value_array(array);
}

void print_value(Value value) {
    switch (value.type) {
            case VAL_BOOL:
                printf(AS_BOOL(value) ? "true" : "false");
                break;
            case VAL_FLOATING: printf("%f", AS_FLOATING(value)); break;
            case VAL_INT: printf("%ld", AS_INT(value)); break;
            default: printf("YEET"); break;
        }
}
