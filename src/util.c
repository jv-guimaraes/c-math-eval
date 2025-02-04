#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

int char_to_int(char c) {
    return c - 48;
}

bool is_digit(char c) {
    return (c >= 48) && (c <= 57);
}

char* trim_whitespace(const char* str) {
    if (!str) return NULL;
    
    // Find first non-whitespace character
    const char* start = str;
    while (*start && isspace(*start)) {
        start++;
    }
    
    // If string is all whitespace
    if (!*start) {
        char* result = malloc(1);
        if (result) *result = '\0';
        return result;
    }
    
    // Find last non-whitespace character
    const char* end = str + strlen(str) - 1;
    while (end > start && isspace(*end)) {
        end--;
    }
    
    // Allocate and copy trimmed string
    size_t len = end - start + 1;
    char* result = malloc(len + 1);
    if (!result) return NULL;
    
    strncpy(result, start, len);
    result[len] = '\0';
    
    return result;
}