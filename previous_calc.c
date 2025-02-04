#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "calc.h"
#include "util.h"

Tokens *tokens;

int base() {
    int value;
    if (match_value(tokens, NUM, &value)) {
        return value;
    }
    
    if (match(tokens, LPAREN)) {
        value = add();
        check(match(tokens, RPAREN), "Error: Expected RPAREN\n");
        return value;
    }

    check(false, "Error: Expected NUM or LPAREN\n");
}

int mult() {
    int res = base();
    while(match(tokens, MUL) || match(tokens, DIV)) {
        TokenType type = previous_token(tokens);
        if (type == MUL) {
            res *= mult();
        } else {
            res /= mult();
        }
    }
    return res;
}

int add() {
    int res = mult();
    while(match(tokens, ADD) || match(tokens, SUB)) {
        TokenType type = previous_token(tokens);
        if (type == ADD) {
            res += add();
        } else {
            res -= add();
        }
    }
    return res;
}

int eval(char *exp) {
    tokens = tokenize(trim_whitespace(exp));
    int res = add();
    check(match(tokens, END), "Error: Expected EOF\n");
    // printf("It passed!\n");
    return res;
}

void check(bool condition, const char *msg) {
    if (!condition) {
        fprintf(stderr, msg);
        exit(1);
    }
}

int main() {
    test_add_mul();
    test_sub_div();
    return 0;
}