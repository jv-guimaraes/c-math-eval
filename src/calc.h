#ifndef CALC_H
#define CALC_H

#include <stdio.h>
#include <assert.h>
#include "tokenizer.h"

Tokens *tokens;

int base();

int mult();

int add();

void test_add_mul();

void test_sub_div();

#endif