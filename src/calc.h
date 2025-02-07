#ifndef CALC_H
#define CALC_H

#include <stdbool.h>

typedef enum NodeType {
    NODE_NUM, NODE_ADD, NODE_MUL, NODE_PARENS
} NodeType;

typedef struct Node {
    NodeType type;
    int value;
    bool opposite; // Is opposite or reciprocal
    struct Node *parent;
    struct Node *child;
    struct Node *sibling;
} Node;

int eval(char *exp, Node **root);

void print_tree(Node *root);

#endif