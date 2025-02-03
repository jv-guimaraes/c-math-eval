#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "util.h"

typedef struct Tokenizer {
    const char *data;
    size_t cursor;
} Tokenizer;

typedef enum TokenType {
    NUM, ADD, SUB, MUL, DIV
} TokenType;

typedef struct Token {
    TokenType type;
    int value;
} Token;

Token Token_new(const char *data, size_t start, size_t end) {
    if (is_digit(data[start])) {
        int total = 0;
        int magnitude = 1;
        for (int i = end - 1; i >= (int) start; i--) {
            total += char_to_int(data[i]) * magnitude;
            magnitude *= 10;
        }
        return (Token){NUM, total};
    } else {
        char op = data[start];
        switch (op) {
            case '+': return (Token){ADD, 0};  break;
            case '-': return (Token){SUB, 0};  break;
            case '*': return (Token){MUL, 0}; break;
            case '/': return (Token){DIV, 0};  break;        
        }
    }
}

void Token_print(Token t) {
    const char *names[] = {"NUM", "ADD", "SUB", "MUL", "DIV"};
    if (t.type == NUM) {
        printf("{%s, %d} ", names[t.type], t.value);    
    } else {
        printf("{%s} ", names[t.type]);   
    }
}

Token Tokenizer_next(Tokenizer *t) {
    size_t start, end;

    // Eat all the whitespace
    start = t->cursor;
    while (t->data[start] == ' ') {
        start++;
    }

    // Form the token
    end = start;
    while (t->data[end] != ' ' && t->data[end] != '\0') {
        end++;
    }
    t->cursor = end;
    return Token_new(t->data, start, end);
}

bool Tokenizer_done(Tokenizer *t) {
    return t->data[t->cursor] == '\0';
}

#endif