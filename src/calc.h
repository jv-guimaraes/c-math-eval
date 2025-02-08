#ifndef CALC_H
#define CALC_H

#include <stdbool.h>

typedef enum NodeType {
    NODE_NUM, NODE_ADD, NODE_MUL
} NodeType;

typedef enum NodeValueType {
    NORMAL, OPPOSITE, RECIPROCAL
} NodeValueType;

typedef struct Node {
    NodeType type;
    int value;
    NodeValueType valueType;
    struct Node *parent;
    struct Node *child;
    struct Node *sibling;
} Node;

int eval(char *exp, Node **root);

void print_tree(Node *root);

void print_tree_ex(Node *root);

#endif