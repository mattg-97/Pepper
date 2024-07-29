#include "common.h"

static void repl() {

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
