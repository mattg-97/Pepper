#include "common.h"

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

static void runFile(const char* fileName) {

}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: pepper [path]\n");
        exit(64);
    }
    return 0;
}
