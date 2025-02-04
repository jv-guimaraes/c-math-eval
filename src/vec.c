#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "vec.h"

Vec Vec_new(size_t initialCapacity) {
    Vec vec;
    vec.array = (Token *)malloc(initialCapacity * sizeof(Token));
    vec.size = 0;
    vec.capacity = initialCapacity;
    return vec;
}

void Vec_append(Vec *vec, Token element) {
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->array = (Token *)realloc(vec->array, vec->capacity * sizeof(Token));
    }
    vec->array[vec->size++] = element;
}

void Vec_free(Vec *vec) {
    free(vec->array);
    vec->array = NULL;
    vec->size = vec->capacity = 0;
}

void Vec_print(Vec *vec) {
    for (size_t i = 0; i < vec->size; i++)
        Token_print(vec->array[i]);
    printf("\n");
}