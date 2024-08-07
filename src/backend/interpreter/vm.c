#include "vm.h"
#include "memory.h"
#include "debug.h"

static void reset_stack(VM* vm) {
    vm->stack_top = vm->stack;
}

VM* init_vm(Chunk* chunk) {
    VM* vm = ALLOCATE(VM, 1);
    reset_stack(vm);
    vm->chunk = chunk;
    initTable(&vm->strings);
    initTable(&vm->globals);
    vm->ip = vm->chunk->code;
    return vm;
}

void free_vm(VM* vm) {
    reset_stack(vm);
    freeTable(&vm->globals);
    freeTable(&vm->strings);
}

static Value pop(VM* vm) {
    vm->stack_top--;
    return *vm->stack_top;
}

static void push(VM* vm, Value value) {
    *vm->stack_top = value;
    vm->stack_top++;
}

Result run(VM* vm) {
    #define READ_BYTE() (*vm->ip++)
    #define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
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
            u64 a = AS_INT(pop(vm));
            u64 b = AS_INT(pop(vm));
            push(vm, INT_VAL(a + b));
            break;
        }
        case OP_POP: {
            pop(vm);
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
    return OK;
}