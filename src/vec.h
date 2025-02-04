#ifndef VEC_H
#define VEC_H

#include "tokenizer.h"

typedef struct {
    Token *array;   
    size_t size;    
    size_t capacity;
} Vec;

Vec Vec_new(size_t initialCapacity);

void Vec_append(Vec *vec, Token element);

void Vec_free(Vec *vec);

void Vec_print(Vec *vec);

#endif