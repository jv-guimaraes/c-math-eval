#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

int char_to_int(char c);

bool is_digit(char c);

char* trim_whitespace(const char* str);

#endif