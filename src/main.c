#include <stdio.h>
#include <assert.h>
#include "calc.h"

void test_eval() {
    // Basic arithmetic
    assert(eval("1 + 2") == 3.0);
    assert(eval("2 - 1") == 1.0);
    assert(eval("2 * 3") == 6.0);
    assert(eval("6 / 2") == 3.0);
    
    // Operator precedence
    assert(eval("1 + 2 * 3") == 7.0);
    assert(eval("2 * 3 + 4") == 10.0);
    assert(eval("2 + 3 * 4") == 14.0);
    assert(eval("10 - 2 * 3") == 4.0);
    
    // Parentheses
    assert(eval("(1 + 2) * 3") == 9.0);
    assert(eval("2 * (3 + 4)") == 14.0);
    assert(eval("(2 + 3) * (4 + 5)") == 45.0);
    assert(eval("(10 + 2) / (3 + 3)") == 2.0);
    
    // Unary minus
    assert(eval("-2") == -2.0);
    assert(eval("-2 + 3") == 1.0);
    assert(eval("-(2 + 3)") == -5.0);
    assert(eval("-2 * 3") == -6.0);
    assert(eval("2 * -3") == -6.0);
    
    // Complex expressions
    assert(eval("1 + 2 * 3 - 4 / 2") == 5.0);
    assert(eval("(1 + 2) * (3 - 4) / 2") == -1.5);
    assert(eval("2 * 3 + 4 * 5") == 26.0);
    assert(eval("(2 + 3) * 4 - 6 / 2") == 17.0);
    
    // Decimal numbers
    assert(eval("1.5 + 2.5") == 4.0);
    assert(eval("2.5 * 3") == 7.5);
    assert(eval("10.0 / 2.5") == 4.0);
    assert(eval("3.14159 + 2.71828") == 5.85987);
    
    // Multiple operations
    assert(eval("1 + 2 + 3 + 4") == 10.0);
    assert(eval("1 - 2 - 3 - 4") == -8.0);
    assert(eval("2 * 2 * 2 * 2") == 16.0);
    assert(eval("16 / 4 / 2") == 2.0);
    
    // Mixed complex expressions
    assert(eval("2 * (3 + 4 * (5 - 2)) - 6") == 24.0);
    assert(eval("(1 + 2) * 3 - 4 * (5 + 6)") == -35.0);
    assert(eval("10 / (2 + 3) * 4") == 8.0);
    assert(eval("-2 * (3 + -4 * 2) + 7") == 17.0);
    
    // Edge cases
    assert(eval("0") == 0.0);
    assert(eval("-0") == 0.0);
    assert(eval("1") == 1.0);
    assert(eval("((1))") == 1.0);
    
    printf("All tests passed successfully!\n");
}

int main() {
    test_eval();

    const char *expr = "1 + 2 * 3";
    ASTNode *ast = ast_build("1 + 2 * 3");
    printf("%s = %.2f\n", expr, ast_eval(ast));
    ast_print(ast, 0);
    return 0;
}
