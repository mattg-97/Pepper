#include "asserts.h"

bool assert_eq_int(int a, int b) {
    return a == b;
}

bool assert_eq_float(float a, float b) {
    return fabsf(a - b) < (float)1e-6;
}

bool assert_eq_double(double a, double b) {
    return fabs(a - b) < 1e-6;
}

bool assert_eq_str(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

bool assert_eq_token_type(TokenType a, TokenType b) {
    return a == b;
}