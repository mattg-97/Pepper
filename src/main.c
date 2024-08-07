#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "vm.h"

static void repl() {
    char line[1024];
    for (;;) {
        printf(">> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        Lexer* lexer = init_lexer(line);
        tokenize(lexer);
        Parser* parser = init_parser(lexer);
        parse_program(parser);
        de_init_lexer(lexer);
    }
}

static char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    u64 file_size = (u64)ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    u64 bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}


static void run_file(const char* path) {
    char* source = read_file(path);
    Lexer* lexer = init_lexer(source);
    tokenize(lexer);
    Parser* parser = init_parser(lexer);
    Program* program = parse_program(parser);
    Chunk* chunk = interpret_program(program);
    VM* vm = init_vm(chunk);
    run(vm);
    de_init_lexer(lexer);
    free(source);
    // exit codes differ for each error
    //if (result == INTERPRET_COMPILE_ERROR) exit(65);
    //if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: pepper [path]\n");
        exit(64);
    }
    return 0;
}
