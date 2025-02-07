#include <stdio.h>
#include <assert.h>
#include "calc.h"

Node *tree;

void test_add_mul() {
    // Basic addition and multiplication
    assert(eval("5 + 6", &tree) == 11);
    assert(eval("3 * 5", &tree) == 15);

    // Whitespace handling
    assert(eval("  5 +  6 ", &tree) == 11);
    assert(eval("3   *   5", &tree) == 15);
    assert(eval("1 + 2 * 3", &tree) == 7);

    // Parentheses handling
    assert(eval("(5 + 6)", &tree) == 11);
    assert(eval("(3 * 5)", &tree) == 15);
    assert(eval("(1 + 2) * 3", &tree) == 9);
    assert(eval("1 + (2 * 3)", &tree) == 7);
    assert(eval("(1 + 2) * (3 + 4)", &tree) == 21);
    assert(eval("((1 + 2) * 3)", &tree) == 9);
    assert(eval("(3) * (5)", &tree) == 15);
    assert(eval("((3) * (5))", &tree) == 15);

    // Multiple operations
    assert(eval("1 + 2 + 3 + 4", &tree) == 10);
    assert(eval("1 * 2 * 3 * 4", &tree) == 24);
    assert(eval("1 + 2 * 3 + 4", &tree) == 11);
    assert(eval("1 * 2 + 3 * 4", &tree) == 14);
    assert(eval("1 * (2 + 3) * 4", &tree) == 20);

    // Edge cases with single numbers
    assert(eval("5", &tree) == 5);
    assert(eval("(5)", &tree) == 5);
    assert(eval(" 5 ", &tree) == 5);
    assert(eval("( 5 )", &tree) == 5);

    // Repeated operations
    assert(eval("1 + 1 + 1 + 1 + 1", &tree) == 5);
    assert(eval("1 * 1 * 1 * 1 * 1", &tree) == 1);
    assert(eval("2 * 2 * 2 * 2", &tree) == 16);
    assert(eval("2 + 2 + 2 + 2", &tree) == 8);

    // Nested parentheses
    assert(eval("((1 + 2) * (3 + 4))", &tree) == 21);
    assert(eval("(1 + (2 * 3)) + 4", &tree) == 11);
    assert(eval("1 + (2 * (3 + 4))", &tree) == 15);
    assert(eval("((1 + 2) * 3) + 4", &tree) == 13);

    // Large numbers
    assert(eval("1000 + 2000", &tree) == 3000);
    assert(eval("1000 * 2000", &tree) == 2000000);
    assert(eval("1000 + 2000 * 3", &tree) == 7000);
    assert(eval("(1000 + 2000) * 3", &tree) == 9000);

    // Zero handling
    assert(eval("0 + 0", &tree) == 0);
    assert(eval("0 * 0", &tree) == 0);
    assert(eval("0 + 5", &tree) == 5);
    assert(eval("5 * 0", &tree) == 0);
    assert(eval("0 * 5 + 3", &tree) == 3);
    assert(eval("0 + 5 * 3", &tree) == 15);
    printf("All add & mul tests are passing!\n");
}

void test_sub_div() {
    // Basic subtraction and division
    assert(eval("10 - 4", &tree) == 6);
    assert(eval("15 / 3", &tree) == 5);

    // Mixed operations
    assert(eval("10 - 4 + 2", &tree) == 8);
    assert(eval("15 / 3 * 2", &tree) == 10);
    assert(eval("10 - 4 * 2", &tree) == 2); // Precedence: multiplication before subtraction
    assert(eval("15 / 3 + 2", &tree) == 7); // Precedence: division before addition
    assert(eval("10 + 4 - 2", &tree) == 12);
    assert(eval("15 * 3 / 5", &tree) == 9); // 45 / 5 = 9

    // Parentheses handling
    assert(eval("(10 - 4) + 2", &tree) == 8);
    assert(eval("10 - (4 + 2)", &tree) == 4);
    assert(eval("(15 / 3) * 2", &tree) == 10);
    assert(eval("15 / (3 * 2)", &tree) == 2); // 15 / 6 = 2 (integer division)
    assert(eval("(10 + 4) - 2", &tree) == 12);
    assert(eval("10 + (4 - 2)", &tree) == 12);
    assert(eval("(15 * 3) / 5", &tree) == 9); // 45 / 5 = 9
    assert(eval("15 * (3 / 5)", &tree) == 0); // 3 / 5 = 0, then 15 * 0 = 0

    // Division edge cases
    assert(eval("10 / 2", &tree) == 5);
    assert(eval("10 / 1", &tree) == 10);
    assert(eval("0 / 5", &tree) == 0); // Division by non-zero
    assert(eval("5 / 10", &tree) == 0); // Integer division: 5 / 10 = 0
    assert(eval("10 / 3", &tree) == 3); // Integer division: 10 / 3 = 3
    assert(eval("7 / 3", &tree) == 2); // Integer division: 7 / 3 = 2
    assert(eval("3 / 5", &tree) == 0); // Integer division: 3 / 5 = 0
    assert(eval("6 / 5", &tree) == 1); // Integer division: 6 / 5 = 1

    // Subtraction edge cases
    assert(eval("10 - 10", &tree) == 0);
    assert(eval("0 - 5", &tree) == -5); // Negative result (if supported)
    assert(eval("5 - 0", &tree) == 5);
    assert(eval("5 - 10", &tree) == -5); // Negative result (if supported)

    // Mixed operations with parentheses
    assert(eval("(10 - 4) * 2", &tree) == 12);
    assert(eval("10 - (4 * 2)", &tree) == 2);
    assert(eval("(15 / 3) - 2", &tree) == 3);
    assert(eval("15 / (3 - 2)", &tree) == 15); // 15 / 1 = 15
    assert(eval("(10 + 4) / 2", &tree) == 7); // 14 / 2 = 7
    assert(eval("10 + (4 / 2)", &tree) == 12); // 4 / 2 = 2, then 10 + 2 = 12
    assert(eval("(15 * 3) - 5", &tree) == 40); // 45 - 5 = 40
    assert(eval("15 * (3 - 5)", &tree) == -30); // 3 - 5 = -2, then 15 * -2 = -30 (if supported)

    // Nested parentheses
    assert(eval("((10 - 4) * 2) + 5", &tree) == 17); // (6 * 2) + 5 = 17
    assert(eval("10 - ((4 * 2) + 5)", &tree) == -3); // 10 - (8 + 5) = -3 (if supported)
    assert(eval("((15 / 3) - 2) * 4", &tree) == 12); // (5 - 2) * 4 = 12
    assert(eval("15 / ((3 - 2) * 5)", &tree) == 3); // 15 / (1 * 5) = 3
    assert(eval("((10 + 4) / 2) - 3", &tree) == 4); // (14 / 2) - 3 = 4
    assert(eval("10 + ((4 / 2) * 5)", &tree) == 20); // 10 + (2 * 5) = 20

    // Whitespace handling
    assert(eval("  10 -  4 ", &tree) == 6);
    assert(eval("15   /   3", &tree) == 5);
    assert(eval("( 10 - 4 ) + 2", &tree) == 8);
    assert(eval("10 - ( 4 + 2 )", &tree) == 4);

    // Repeated operations
    assert(eval("10 - 4 - 2", &tree) == 4); // Left-associative subtraction: 10 - 4 = 6, then 6 - 2 = 4
    assert(eval("15 / 3 / 2", &tree) == 2); // Left-associative division: 15 / 3 = 5, then 5 / 2 = 2
    assert(eval("10 - 4 - 2 - 1", &tree) == 3); // 10 - 4 = 6, 6 - 2 = 4, 4 - 1 = 3
    assert(eval("16 / 2 / 2 / 2", &tree) == 2); // 16 / 2 = 8, 8 / 2 = 4, 4 / 2 = 2

    // Large numbers
    assert(eval("1000 - 500", &tree) == 500);
    assert(eval("1000 / 10", &tree) == 100);
    assert(eval("1000 - 500 + 200", &tree) == 700);
    assert(eval("1000 / 10 * 5", &tree) == 500); // 1000 / 10 = 100, then 100 * 5 = 500

    // Zero handling
    assert(eval("0 - 0", &tree) == 0);
    assert(eval("0 / 5", &tree) == 0);
    assert(eval("5 - 0", &tree) == 5);
    assert(eval("0 / 1", &tree) == 0);
    assert(eval("0 - 5", &tree) == -5); // Negative result (if supported)
    assert(eval("5 / 1", &tree) == 5);

    printf("All sub and div tests are passing!\n");
}

int main() {
    test_add_mul();
    test_sub_div();
    eval("10 / 5 - 2", &tree);
    printf("Result: %d\n", tree->value);
    print_tree(tree);
    return 0;
}