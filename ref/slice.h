#ifndef SLICE_H
#define SLICE_H

#include <stdlib.h>
#include "tokenizer.h"

typedef struct Slice {
    Token *data;
    size_t len;
} Slice;

void Slice_popback(Slice *slice) {
    slice->len--;
    slice->data++;
}

#endif