#include "common.h"
#include "utils/logger.h"
#include "frontend/lexer.h"

static void repl() {
    char line[1024];
    for (;;) {
        // print a helper character at the start of each repl line
        printf("-> ");
        // get stdin and load into char array
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        // interpret that line
        printf("%s\n", line);
    }

}

static char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}


static void run_file(const char* path) {
    // read in the file
    char* source = read_file(path);
    // interpret the source code
    init_lexer(source);
    tokenize();
    // then free the char array (we need to do this because readfile dynamically
    // allocates the memory and then passes ownership to this function)
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
