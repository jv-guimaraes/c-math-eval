#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "vec.h"

typedef struct Tokenizer {
    const char *data;
    size_t cursor;
} Tokenizer;

/* Token */
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
            case '+': return (Token){ADD, 0};     break;
            case '-': return (Token){SUB, 0};     break;
            case '*': return (Token){MUL, 0};     break;
            case '/': return (Token){DIV, 0};     break;      
            case '(': return (Token){LPAREN, 0};  break;      
            case ')': return (Token){RPAREN, 0};  break;      
        }
    }
}

void Token_print(Token t) {
    const char *names[] = {"NUM", "ADD", "SUB", "MUL", "DIV", "END", "LPAREN", "RPAREN"};
    if (t.type == NUM) {
        printf("{%s, %d} ", names[t.type], t.value);    
    } else {
        printf("{%s} ", names[t.type]);   
    }
}

/* Tokenizer */
Token Tokenizer_next(Tokenizer *t) {
    size_t start, end;

    // Eat all the whitespace
    start = t->cursor;
    while (t->data[start] == ' ') start++;

    // Form the token
    end = start;
    if (is_digit(t->data[start])) {
        while (t->data[end] != ' ' && t->data[end] != '\0' && is_digit(t->data[end])) {
            end++;
        }
    } else {
        end++;
    }
    Token token = Token_new(t->data, start, end);
    t->cursor = end;

    // Eat all the whitespace
    start = t->cursor;
    while (t->data[start] == ' ') start++;

    return token;
}

bool Tokenizer_done(Tokenizer *t) {
    return t->data[t->cursor] == '\0';
}

/* Tokens */
Tokens *Tokens_new(Token *data, size_t size) {
    Tokens *tokens = malloc(sizeof(Tokens));
    tokens->data = data;
    tokens->size = size;
    tokens->cursor = 0;
    return tokens;
}

void Tokens_print(Tokens *tokens) {
    for (size_t i = 0; i < tokens->size; i++) {
        Token token = tokens->data[i];
        Token_print(token);
    }
    printf("\n");
}

Tokens *tokenize(const char *exp) {
    Tokenizer tokenizer = {exp, 0};
    Vec tokens = Vec_new(8);
    while (!Tokenizer_done(&tokenizer)) {
        Vec_append(&tokens, Tokenizer_next(&tokenizer));
    }
    Vec_append(&tokens, (Token){END, 0});
    return Tokens_new(tokens.array, tokens.size);
}

Token peek(const Tokens *tokens) {
    Token token = tokens->data[0];
}

Token next(Tokens *tokens) {
    Token token = tokens->data[tokens->cursor];
    tokens->cursor++;
    return token;
}

bool match(Tokens *tokens, TokenType type) {
    if (tokens->data[tokens->cursor].type == type) {
        next(tokens);
        return true;
    }
    return false;
}

bool match_value(Tokens *tokens, TokenType type, int *value) {
    Token token = tokens->data[tokens->cursor];
    if (token.type == type) {
        *value = token.value;
        next(tokens);
        return true;
    }
    return false;
}

TokenType previous_token(Tokens *tokens) {
    return tokens->data[tokens->cursor - 1].type;
}
