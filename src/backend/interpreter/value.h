#ifndef pepper_value_h
#define pepper_value_h

#include "common.h"

typedef enum {
    VAL_BOOL,
    VAL_INT,
    VAL_FLOATING,
    VAL_STRING,
    VAL_NIL,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        i64 integer;
        f64 floating;
        char* string;
    } as;
} Value;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_INT(value) ((value).type == VAL_INT)
#define IS_STRING(value) ((value).type == VAL_STRING)
#define IS_NIL(value) ((value).type == VAL_NIL)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_INT(value) ((value).as.integer)
#define AS_STRING(value) ((value).as.string)
#define AS_FLOATING(value) ((value).as.floating)

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define INT_VAL(value) ((Value){VAL_INT, {.integer = value}})
#define FLOATING_VAL(value) ((Value){VAL_FLOATING, {.floating = value}})
#define STRING_VAL(value) ((Value){VAL_STRING, {.string = value}})
#define NIL_VAL ((Value){VAL_NIL, {.boolean = false}})

typedef struct {
    u64 capacity;
    u64 count;
    Value* values;
} ValueArray;

bool values_equal(Value a, Value b);
void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);
void print_value(Value value);

#endif
