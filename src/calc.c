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

void test_add_mul() {
    // Basic addition and multiplication
    assert(eval("5 + 6") == 11);
    assert(eval("3 * 5") == 15);

    // Whitespace handling
    assert(eval("  5 +  6 ") == 11);
    assert(eval("3   *   5") == 15);
    assert(eval("1 + 2 * 3") == 7);

    // Parentheses handling
    assert(eval("(5 + 6)") == 11);
    assert(eval("(3 * 5)") == 15);
    assert(eval("(1 + 2) * 3") == 9);
    assert(eval("1 + (2 * 3)") == 7);
    assert(eval("(1 + 2) * (3 + 4)") == 21);
    assert(eval("((1 + 2) * 3)") == 9);
    assert(eval("(3) * (5)") == 15);
    assert(eval("((3) * (5))") == 15);

    // Multiple operations
    assert(eval("1 + 2 + 3 + 4") == 10);
    assert(eval("1 * 2 * 3 * 4") == 24);
    assert(eval("1 + 2 * 3 + 4") == 11);
    assert(eval("1 * 2 + 3 * 4") == 14);
    assert(eval("1 * (2 + 3) * 4") == 20);

    // Edge cases with single numbers
    assert(eval("5") == 5);
    assert(eval("(5)") == 5);
    assert(eval(" 5 ") == 5);
    assert(eval("( 5 )") == 5);

    // Repeated operations
    assert(eval("1 + 1 + 1 + 1 + 1") == 5);
    assert(eval("1 * 1 * 1 * 1 * 1") == 1);
    assert(eval("2 * 2 * 2 * 2") == 16);
    assert(eval("2 + 2 + 2 + 2") == 8);

    // Nested parentheses
    assert(eval("((1 + 2) * (3 + 4))") == 21);
    assert(eval("(1 + (2 * 3)) + 4") == 11);
    assert(eval("1 + (2 * (3 + 4))") == 15);
    assert(eval("((1 + 2) * 3) + 4") == 13);

    // Large numbers
    assert(eval("1000 + 2000") == 3000);
    assert(eval("1000 * 2000") == 2000000);
    assert(eval("1000 + 2000 * 3") == 7000);
    assert(eval("(1000 + 2000) * 3") == 9000);

    // Zero handling
    assert(eval("0 + 0") == 0);
    assert(eval("0 * 0") == 0);
    assert(eval("0 + 5") == 5);
    assert(eval("5 * 0") == 0);
    assert(eval("0 * 5 + 3") == 3);
    assert(eval("0 + 5 * 3") == 15);
    printf("All add & mul tests are passing!\n");
}

void test_sub_div() {
    // Basic subtraction and division
    assert(eval("10 - 4") == 6);
    assert(eval("15 / 3") == 5);

    // Mixed operations
    assert(eval("10 - 4 + 2") == 8);
    assert(eval("15 / 3 * 2") == 10);
    assert(eval("10 - 4 * 2") == 2); // Precedence: multiplication before subtraction
    assert(eval("15 / 3 + 2") == 7); // Precedence: division before addition
    assert(eval("10 + 4 - 2") == 12);
    assert(eval("15 * 3 / 5") == 9);

    // Parentheses handling
    assert(eval("(10 - 4) + 2") == 8);
    assert(eval("10 - (4 + 2)") == 4);
    assert(eval("(15 / 3) * 2") == 10);
    assert(eval("15 / (3 * 2)") == 2); // Precedence: parentheses first
    assert(eval("(10 + 4) - 2") == 12);
    assert(eval("10 + (4 - 2)") == 12);
    assert(eval("(15 * 3) / 5") == 9);
    assert(eval("15 * (3 / 5)") == 9);

    // Division edge cases
    assert(eval("10 / 2") == 5);
    assert(eval("10 / 1") == 10);
    assert(eval("0 / 5") == 0); // Division by non-zero
    assert(eval("5 / 10") == 0); // Integer division (assuming your function uses integer division)
    assert(eval("10 / 3") == 3); // Integer division (assuming your function uses integer division)

    // Subtraction edge cases
    assert(eval("10 - 10") == 0);
    assert(eval("0 - 5") == -5); // Negative result (if supported)
    assert(eval("5 - 0") == 5);
    assert(eval("5 - 10") == -5); // Negative result (if supported)

    // Mixed operations with parentheses
    assert(eval("(10 - 4) * 2") == 12);
    assert(eval("10 - (4 * 2)") == 2);
    assert(eval("(15 / 3) - 2") == 3);
    assert(eval("15 / (3 - 2)") == 15);
    assert(eval("(10 + 4) / 2") == 7);
    assert(eval("10 + (4 / 2)") == 12);
    assert(eval("(15 * 3) - 5") == 40);
    assert(eval("15 * (3 - 5)") == -30); // Negative result (if supported)

    // Nested parentheses
    assert(eval("((10 - 4) * 2) + 5") == 17);
    assert(eval("10 - ((4 * 2) + 5)") == -3); // Negative result (if supported)
    assert(eval("((15 / 3) - 2) * 4") == 12);
    assert(eval("15 / ((3 - 2) * 5)") == 3);
    assert(eval("((10 + 4) / 2) - 3") == 4);
    assert(eval("10 + ((4 / 2) * 5)") == 20);

    // Whitespace handling
    assert(eval("  10 -  4 ") == 6);
    assert(eval("15   /   3") == 5);
    assert(eval("( 10 - 4 ) + 2") == 8);
    assert(eval("10 - ( 4 + 2 )") == 4);

    // Repeated operations
    assert(eval("10 - 4 - 2") == 4); // Left-associative subtraction
    assert(eval("15 / 3 / 2") == 2); // Left-associative division
    assert(eval("10 - 4 - 2 - 1") == 3);
    assert(eval("16 / 2 / 2 / 2") == 2);

    // Division by zero (if error handling is implemented)
    // Note: If your function handles division by zero, add these tests:
    // assert(eval("10 / 0") == ERROR); // Division by zero
    // assert(eval("(10 - 10) / 0") == ERROR); // Division by zero

    // Large numbers
    assert(eval("1000 - 500") == 500);
    assert(eval("1000 / 10") == 100);
    assert(eval("1000 - 500 + 200") == 700);
    assert(eval("1000 / 10 * 5") == 500);

    // Zero handling
    assert(eval("0 - 0") == 0);
    assert(eval("0 / 5") == 0);
    assert(eval("5 - 0") == 5);
    assert(eval("0 / 1") == 0);
    assert(eval("0 - 5") == -5); // Negative result (if supported)
    assert(eval("5 / 1") == 5);
}