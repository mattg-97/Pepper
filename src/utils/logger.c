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
    const char* level_strings[6] =  {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    bool is_error = level < LOG_LEVEL_WARN;

    const i32 message_length = 32000;
    char out_message[message_length];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, message_length, message, arg_ptr);
    va_end(arg_ptr);

    char out_message_2[message_length];
    snprintf(out_message_2, message_length, "%s%s\n", level_strings[level], out_message);
    if (is_error) {
        fprintf(stderr, "%s\n", out_message_2);
        exit(1);
    } else {
        fprintf(stdout, "%s\n", out_message_2);
    }
}
