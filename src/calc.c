#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define MAX_TOKENS 128

// --- Data Structures ---
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

typedef struct Tokenizer {
    const char *data;
    size_t cursor;
} Tokenizer;

// --- Utility functions ---
int char_to_int(char c) {
    return c - 48;
}

bool is_digit(char c) {
    return (c >= '0') && (c <= '9');
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

// --- Tokenizer ---
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
    //Should never reach here, but added for safety.  Indicates an error.
    return (Token){END, 0};
}

void Token_print(Token t) {
    const char *names[] = {"NUM", "ADD", "SUB", "MUL", "DIV", "END", "LPAREN", "RPAREN"};
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

bool Tokenizer_isDone(Tokenizer *t) {
    return t->data[t->cursor] == '\0';
}

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
    Token *tokens_array = malloc(MAX_TOKENS * sizeof(Token));
    if (!tokens_array) return NULL;

    Tokenizer tokenizer = {exp, 0};
    size_t token_count = 0;

    while (!Tokenizer_isDone(&tokenizer)) {
        if (token_count >= MAX_TOKENS - 1) {
            fprintf(stderr, "Error: Expression too long\n");
            free(tokens_array);
            return NULL;
        }
        tokens_array[token_count++] = Tokenizer_next(&tokenizer);
    }

    tokens_array[token_count++] = (Token){END, 0};
    return Tokens_new(tokens_array, token_count);
}

Token Tokens_next(Tokens *tokens) {
    Token token = tokens->data[tokens->cursor];
    tokens->cursor++;
    return token;
}

void Tokens_free(Tokens *tokens) {
    free(tokens->data);
    free(tokens);
}

// --- Parser ---
bool match(Tokens *tokens, TokenType type) {
    if (tokens->cursor < tokens->size && tokens->data[tokens->cursor].type == type) {
        Tokens_next(tokens);
        return true;
    }
    return false;
}

bool match_value(Tokens *tokens, TokenType type, int *value) {
    if (tokens->cursor < tokens->size && tokens->data[tokens->cursor].type == type) {
        *value = tokens->data[tokens->cursor].value;
        Tokens_next(tokens);
        return true;
    }
    return false;
}

TokenType previous_token(Tokens *tokens) {
    if (tokens->cursor > 0)
        return tokens->data[tokens->cursor - 1].type;
    return END; //Or some other default value - should not happen in correct use.
}

int add(Tokens *tokens);

int base(Tokens *tokens) {
    int value;
    if (match_value(tokens, NUM, &value)) {
        return value;
    }

    if (match(tokens, LPAREN)) {
        value = add(tokens);
        if (!match(tokens, RPAREN)) {
            fprintf(stderr, "Error: Expected RPAREN\n");
            exit(1);
        }
        return value;
    }

    fprintf(stderr, "Error: Expected NUM or LPAREN\n");
    exit(1);
}

int mult(Tokens *tokens) {
    int res = base(tokens);
    while (match(tokens, MUL) || match(tokens, DIV)) {
        TokenType type = previous_token(tokens);
        if (type == MUL) {
            res *= base(tokens);
        } else {
            res /= base(tokens);
        }
    }
    return res;
}

int add(Tokens *tokens) {
    int res = mult(tokens);
    while (match(tokens, ADD) || match(tokens, SUB)) {
        TokenType type = previous_token(tokens);
        if (type == ADD) {
            res += mult(tokens);
        } else {
            res -= mult(tokens);
        }
    }
    return res;
}

int eval(char *exp) {
    Tokens *tokens = tokenize(trim_whitespace(exp));
    int res = add(tokens);
    if (!match(tokens, END)) {
        fprintf(stderr, "Error: Expected EOF\n");
        exit(1);
    }

    Tokens_free(tokens);
    return res;
}

void check(bool condition, const char *msg) {
    if (!condition) {
        fprintf(stderr, msg);
        exit(1);
    }
}
