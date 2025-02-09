#ifndef CALC_H
#define CALC_H

typedef enum {
    NODE_NUMBER, NODE_BINARY_OP, NODE_UNARY_OP
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        double number;  // For NUMBER nodes
        struct {       // For BINARY_OP nodes
            char operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {       // For UNARY_OP nodes
            char operator;
            struct ASTNode* operand;
        } unary;
    };
} ASTNode;

ASTNode *ast_build(const char* expression);

void ast_print(ASTNode* node, int depth);

void ast_free(ASTNode* node);

double ast_eval(ASTNode* node);

double eval(const char* expression);

#endif