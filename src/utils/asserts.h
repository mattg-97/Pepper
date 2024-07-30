#ifndef pepper_asserts_h
#define pepper_asserts_h

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "lexer.h"
// Define comparison functions for different types
bool assert_eq_int(int a, int b);
bool assert_eq_float(float a, float b);
bool assert_eq_double(double a, double b);
bool assert_eq_str(const char* a, const char* b);
bool assert_eq_token_type(TokenType a, TokenType b);

// Define a macro to select the appropriate comparison function
#define ASSERT_EQ(a, b) _Generic((a),       \
    int: assert_eq_int,                     \
    float: assert_eq_float,                 \
    double: assert_eq_double,               \
    const char*: assert_eq_str,             \
    char*: assert_eq_str                    \
    TokenType: assert_eq_token_type         \
)(a, b)

// Helper macro to print error message
#define ASSERT_EQ_MSG(a, b) do {                                             \
    if (!ASSERT_EQ(a, b)) {                                                  \
        fprintf(stderr, "Assertion failed: (%s == %s), file %s, line %d.\n", \
                #a, #b, __FILE__, __LINE__);                                 \
        return false;                                                        \
    }                                                                        \
} while (0)
#endif