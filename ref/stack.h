#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tokenizer.h"

typedef struct Node {
    Token data;
    struct Node* next;
} Node;

typedef struct Stack {
    Node* top;
} Stack;

Node* createNode(Token value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->data = value;
    newNode->next = NULL;
    return newNode;
}


bool isEmpty(Stack* s) {
    return s->top == NULL;
}

void push(Stack* s, Token value) {
    Node* newNode = createNode(value);
    newNode->next = s->top; // Point the new node to the current top
    s->top = newNode;       // Update the top to the new node
    printf("Pushed %d onto the stack.\n", value);
}

Token pop(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack underflow! Cannot pop from an empty stack.\n");
        return (Token){0, -1}; // Return an invalid value to indicate underflow
    }
    Node* temp = s->top;       // Store the current top node
    Token poppedValue = temp->data; // Retrieve the data
    s->top = s->top->next;     // Update the top to the next node
    free(temp);                // Free the memory of the popped node
    return poppedValue;
}

Token peek(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack is empty, cannot peek.\n");
        return (Token){0, -1}; // Return an invalid value to indicate empty stack
    }
    return s->top->data;
}

void display(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack is empty.\n");
        return;
    }
    printf("Stack elements: ");
    Node* current = s->top;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

void freeStack(Stack* s) {
    while (!isEmpty(s)) {
        pop(s); // Pop all elements to free memory
    }
}

#endif
