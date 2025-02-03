#include <stdio.h>
#include "util.h"
#include "tokenizer.h"
#include "stack.h"

int _eval(Tokenizer *t) {
    int lside = Tokenizer_next(t).value;
    // "8 - 5 + 1"
    while (!Tokenizer_done(t)) {
        TokenType operation = Tokenizer_next(t).type;
        if (operation == ADD || operation == SUB) {
            int rside = _eval(t);
            switch (operation) {
                case ADD:  return lside + rside;
                case SUB:  return lside - rside;
            }
        } else {
            switch (operation) {
                case MUL:  lside *= Tokenizer_next(t).value; break;
                case DIV:  lside /= Tokenizer_next(t).value; break;
            }
        }
    }

    return lside;
}

int eval(const char *exp) {
    Tokenizer t = {exp, 0};
    return _eval(&t);
}

int main() {
    // printf("result: %d\n", eval("1"));
    // printf("result: %d\n", eval("3 + 1"));
    // printf("result: %d\n\n", eval("2 + 3 + 1"));

    // printf("result: %d\n", eval("2 * 3"));
    // printf("result: %d\n", eval("2 * 3 * 2"));
    // printf("result: %d\n", eval("2 + 3 * 2"));
    // printf("result: %d\n", eval("2 * 3 + 2"));
    printf("result: %d\n", eval("2 * 2 * 2 - 5 + 1"));
    printf("result: %d\n", eval("1 + 5 - 2 * 2 * 2"));
    return 0;
}