#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

int char_to_int(char c) {
    return c - 48;
}

bool is_digit(char c) {
    return (c >= 48) && (c <= 57);
}

#endif