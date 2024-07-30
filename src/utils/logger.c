#include <stdarg.h>
#include <string.h>
#include "logger.h"

bool initialize_logging() {
    return true;
}

void shutdown_logging() {
    //TODO
}

void log_output(LogLevel level, const char* message, ...) {
    const char* levelStrings[6] =  {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    bool isError = level < LOG_LEVEL_WARN;

    const i32 messageLength = 32000;
    char outMessage[messageLength];
    memset(outMessage, 0, sizeof(outMessage));

    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(outMessage, messageLength, message, argPtr);
    va_end(argPtr);

    char outMessage2[messageLength];
    sprintf(outMessage2, "%s%s\n", levelStrings[level], outMessage);
    if (isError) {
        fprintf(stderr, "%s\n", outMessage2);
        exit(1);
    } else {
        fprintf(stdout, "%s\n", outMessage2);
    }
}