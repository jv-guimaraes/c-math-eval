#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>

typedef enum TokenType {
    NUM, ADD, SUB, MUL, DIV, END, LPAREN, RPAREN
} TokenType;

typedef struct Token {
    TokenType type;
    int value;
} Token;

typedef struct Tokens {
    Token *data;
    size_t size;
    size_t cursor;
} Tokens;

void Token_print(Token t);

Tokens *tokenize(const char *exp);

Token peek(const Tokens *tokens);

Token next(Tokens *tokens);

void Tokens_print(Tokens *tokens);

bool match(Tokens *tokens, TokenType type);

bool match_value(Tokens *tokens, TokenType type, int *value);

void check(bool condition, const char *msg);

TokenType previous_token(Tokens *tokens);

#endif