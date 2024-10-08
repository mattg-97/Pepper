#include <strings.h>

#include "vm.h"
#include "chunk.h"
#include "memory.h"
#include "logger.h"
#include "debug.h"
#include "hashtable.h"
#include "value.h"
#include "bytecode_generator.h"

static void reset_stack(VM* vm) {
    vm->stack_top = vm->stack;
}

VM* init_vm(ByteCode* byte_code) {
    VM* vm = ALLOCATE(VM, 1);
    reset_stack(vm);
    vm->ip = byte_code->chunk->code;
    vm->chunk = byte_code->chunk;
    vm->strings = byte_code->strings;
    vm->globals = byte_code->globals;
    return vm;
}

void free_vm(VM* vm) {
    reset_stack(vm);
    free(vm);
}

static Value pop(VM* vm) {
    vm->stack_top--;
    return *vm->stack_top;
}


static void push(VM* vm, Value value) {
    *vm->stack_top = value;
    vm->stack_top++;
}

static Value* peek(VM* vm, int distance) {
    return &vm->stack_top[-1 - distance];
}

Result run(VM* vm) {
    #define READ_BYTE() (*vm->ip++)
    #define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
    #define READ_STRING() AS_STRING(READ_CONSTANT())
    #define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)
    for (;;) {
    #ifdef DEBUG_MODE_VM
    printf("         ");
    for (Value* slot = vm->stack; slot < vm->stack_top; slot++) {
        printf("[ ");
        print_value(*slot);
        printf(" ]");
    }
    printf("\n");
    // This function takes an integer offset, so we need to do some pointer math to convert
    // ip back to its relative offset from the beginning of the bytecode
    disassemble_instruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(vm, constant);
            break;
        }
        case OP_ADD: {
            i64 a = AS_INT(pop(vm));
            i64 b = AS_INT(pop(vm));
            push(vm, INT_VAL(a + b));
            break;
        }
        case OP_SUBTRACT: {
            i64 b = AS_INT(pop(vm));
            i64 a = AS_INT(pop(vm));
            push(vm, INT_VAL(a - b));
            break;
        }
        case OP_MULTIPLY: {
            i64 b = AS_INT(pop(vm));
            i64 a = AS_INT(pop(vm));
            push(vm, INT_VAL(a * b));
            break;
        }
        case OP_DIVIDE: {
            i64 b = AS_INT(pop(vm));
            i64 a = AS_INT(pop(vm));
            push(vm, INT_VAL(a / b));
            break;
        }
        case OP_NEGATE: {
            push(vm, pop(vm));
            break;
        }
        case OP_POP: {
            pop(vm);
            break;
        }
        case OP_PRINT: {
            print_value(pop(vm));
            break;
        }
        case OP_DEFINE_GLOBAL: {
            char* name = READ_STRING();
            Value* global = (Value*)malloc(sizeof(Value));
            memcpy(global, peek(vm, 0), sizeof(Value));
            if (hash_table_add(vm->globals, name, global) == -1) {
                ERROR("Undefined variable '%s'.", name);
                return RUNTIME_ERROR;
            }
            pop(vm);
            break;
        }
        case OP_SET_GLOBAL: {
            char* name = READ_STRING();
            if (hash_table_get(vm->globals, name) == NULL) {
                ERROR("Undefined variable '%s'.", name);
                return RUNTIME_ERROR;
            }
            pop(vm);
            break;
        }
        case OP_GREATER: {
            i64 b = AS_INT(pop(vm));
            i64 a = AS_INT(pop(vm));
            Value val = {.type = VAL_BOOL};
            if (a > b) {
                val.as.boolean = true;
            } else {
                val.as.boolean = false;
            }
            push(vm, val);
            break;
        }
        case OP_GET_GLOBAL: {
            char* name = READ_STRING();
            Value* value = (Value*)hash_table_get(vm->globals, name);
            if (value == NULL) {
                ERROR("Undefined variable '%s'.", name);
                return RUNTIME_ERROR;
            }
            push(vm, *value);
            break;
        }
        case OP_RETURN:
            return OK;
            break;
        default: break;
    }
    }
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef READ_STRING
    #undef BINARY_OP
    return OK;
}
