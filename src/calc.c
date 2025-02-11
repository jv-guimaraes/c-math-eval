#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "calc.h"

#define ALLOW_INVALID_TREE true

/* ===== Lexer ===== */
typedef enum {
    TOKEN_NUMBER, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_EOF, TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    double value;
} Token;

typedef struct {
    const char* input;
    size_t input_len;
    size_t position;
    char curr_char;
    size_t token_count;
    size_t token_max;
} Lexer;

void tokentype_print(TokenType type) {
    char *tokens[] = {"TOKEN_NUMBER", "TOKEN_PLUS", "TOKEN_MINUS", "TOKEN_MULTIPLY", "TOKEN_DIVIDE",
        "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_EOF", "TOKEN_ERROR"};
    printf("%s\n", tokens[type]);
}

Lexer* lexer_create(const char* input) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->input_len = strlen(input);
    lexer->position = 0;
    lexer->curr_char = input[0];
    lexer->token_count = 0;
    lexer->token_max = SIZE_MAX;
    return lexer;
}

void lexer_advance(Lexer* lexer) {
    lexer->position++;
    if (lexer->position < lexer->input_len) {
        lexer->curr_char = lexer->input[lexer->position];
    } else {
        lexer->curr_char = '\0';
    }
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->curr_char != '\0' && isspace(lexer->curr_char)) {
        lexer_advance(lexer);
    }
}

Token lexer_get_number(Lexer* lexer) {
    char number[256] = {0};
    int i = 0;
    bool has_decimal = false;
    
    while (lexer->curr_char != '\0' && (isdigit(lexer->curr_char) || (lexer->curr_char == '.' && !has_decimal))) {
        if (lexer->curr_char == '.') {
            has_decimal = true;
        }
        number[i++] = lexer->curr_char;
        lexer_advance(lexer);
    }
    
    return (Token){TOKEN_NUMBER, atof(number)};
}

Token lexer_get_next_token(Lexer* lexer) {
    while (lexer->curr_char != '\0' && lexer->token_count < lexer->token_max) {
        if (isspace(lexer->curr_char)) {
            lexer_skip_whitespace(lexer);
            continue;
        }
        
        if (isdigit(lexer->curr_char) || lexer->curr_char == '.') {
            lexer->token_count++;
            return lexer_get_number(lexer);
        }
        
        switch (lexer->curr_char) {
            case '+':
                lexer_advance(lexer); lexer->token_count++;
                return (Token){TOKEN_PLUS, 0};
            case '-':
                lexer_advance(lexer); lexer->token_count++;
                return (Token){TOKEN_MINUS, 0};
            case '*':
                lexer_advance(lexer); lexer->token_count++;
                return (Token){TOKEN_MULTIPLY, 0};
            case '/':
                lexer_advance(lexer); lexer->token_count++;
                return (Token){TOKEN_DIVIDE, 0};
            case '(':
                lexer_advance(lexer); lexer->token_count++;
                return (Token){TOKEN_LPAREN, 0};
            case ')':
                lexer_advance(lexer); lexer->token_count++;
                return (Token){TOKEN_RPAREN, 0};
            default:
                return (Token){TOKEN_ERROR, 0};
        }
    }
    return (Token){TOKEN_EOF, 0};
}

/* ===== Recursive descent parser ===== */
typedef struct {
    Lexer* lexer;
    Token curr_token;
    size_t max_tokens; // Maximum number of tokens to read
} Parser;

ASTNodeList *nodelist_create() {
    ASTNodeList *list = (ASTNodeList*)malloc(sizeof(ASTNodeList));
    memset(list->data, 0, sizeof(list->data));
    list->size = 0;
    return list;
}

void nodelist_append(ASTNodeList *list, ASTNode *node) {
    list->data[list->size] = node;
    list->size++;
}

void nodelist_free(ASTNodeList *list) {
    for (size_t i = 0; i < list->size; i++) {
        free(list->data[i]);
    }
    free(list);
}

ASTNode* astnode_create_number(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUMBER;
    node->number = value;
    return node;
}

ASTNode* astnode_create_binary(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->binary.operator = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* astnode_create_unary(char op, ASTNode* operand) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_UNARY_OP;
    node->unary.operator = op;
    node->unary.operand = operand;
    return node;
}

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->curr_token = lexer_get_next_token(lexer);
    parser->max_tokens = SIZE_MAX;
    return parser;
}

void parser_eat(Parser* parser, TokenType token_type) {
    if (parser->curr_token.type == token_type) {
        parser->curr_token = lexer_get_next_token(parser->lexer);
    } else {
        fprintf(stderr, "Syntax error\n");
        exit(1);
    }
}

ASTNode* parser_expr(Parser* parser);

ASTNode* parser_factor(Parser* parser) {
    Token token = parser->curr_token;
    ASTNode* node;
    
    switch (token.type) {
        case TOKEN_NUMBER:
            parser_eat(parser, TOKEN_NUMBER);
            return astnode_create_number(token.value);
            
        case TOKEN_LPAREN:
            parser_eat(parser, TOKEN_LPAREN);
            node = parser_expr(parser);
            if (ALLOW_INVALID_TREE && parser->curr_token.type != TOKEN_RPAREN) {
                return node;
            }
            parser_eat(parser, TOKEN_RPAREN);
            return node;
            
        case TOKEN_MINUS:
            parser_eat(parser, TOKEN_MINUS);
            return astnode_create_unary('-', parser_factor(parser));
            
        default:
            if (ALLOW_INVALID_TREE) {
                return NULL;
            }
            fprintf(stderr, "Syntax error in factor\n");
            exit(1);
    }
}

ASTNode* parser_term(Parser* parser) {
    ASTNode* node = parser_factor(parser);
    
    while (parser->curr_token.type == TOKEN_MULTIPLY || parser->curr_token.type == TOKEN_DIVIDE) {
        Token token = parser->curr_token;
        char op = (token.type == TOKEN_MULTIPLY) ? '*' : '/';
        parser_eat(parser, token.type);
        node = astnode_create_binary(op, node, parser_factor(parser));
    }
    
    return node;
}

ASTNode* parser_expr(Parser* parser) {
    ASTNode* node = parser_term(parser);
    
    while (parser->curr_token.type == TOKEN_PLUS || parser->curr_token.type == TOKEN_MINUS) {
        Token token = parser->curr_token;
        char op = (token.type == TOKEN_PLUS) ? '+' : '-';
        parser_eat(parser, token.type);
        node = astnode_create_binary(op, node, parser_term(parser));
    }
    
    return node;
}

void ast_print(ASTNode* node, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    if (node == NULL) {
        printf("NULL\n");
        return;
    }
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("Number: %.2f\n", node->number);
            break;
        case NODE_BINARY_OP:
            printf("Binary Op: %c\n", node->binary.operator);
            ast_print(node->binary.left, depth + 1);
            ast_print(node->binary.right, depth + 1);
            break;
        case NODE_UNARY_OP:
            printf("Unary Op: %c\n", node->unary.operator);
            ast_print(node->unary.operand, depth + 1);
            break;
    }
}

double ast_eval(ASTNode* node) {
    if (node == NULL) {
        fprintf(stderr, "Error: NULL node in evaluation\n");
        exit(1);
    }
    
    switch (node->type) {
        case NODE_NUMBER:
            return node->number;   
        case NODE_BINARY_OP: {
            double left = ast_eval(node->binary.left);
            double right = ast_eval(node->binary.right);
            
            switch (node->binary.operator) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0) {
                        fprintf(stderr, "Error: Division by zero\n");
                        exit(1);
                    }
                    return left / right;
                default:
                    fprintf(stderr, "Error: Unknown binary operator %c\n", node->binary.operator);
                    exit(1);
            }
        }
        case NODE_UNARY_OP: {
            double operand = ast_eval(node->unary.operand);
            
            switch (node->unary.operator) {
                case '-': return -operand;
                default:
                    fprintf(stderr, "Error: Unknown unary operator %c\n", node->unary.operator);
                    exit(1);
            }
        }
        default:
            fprintf(stderr, "Error: Unknown node type\n");
            exit(1);
    }
}

void ast_free(ASTNode* node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_BINARY_OP:
            ast_free(node->binary.left);
            ast_free(node->binary.right);
            break;
        case NODE_UNARY_OP:
            ast_free(node->unary.operand);
            break;
        case NODE_NUMBER:
            break;
    }
    
    free(node);
}

ASTNode *ast_build(const char* expression) {
    Lexer* lexer = lexer_create(expression);
    Parser* parser = parser_create(lexer);
    ASTNode* root = parser_expr(parser);

    free(parser);
    free(lexer);
    return root;
}

ASTNode *ast_build_with_token_limit(const char* expression, size_t token_max) {
    Lexer* lexer = lexer_create(expression);
    lexer->token_max = token_max;
    Parser* parser = parser_create(lexer);
    ASTNode* root = parser_expr(parser);

    free(parser);
    free(lexer);
    return root;
}

ASTNodeList *ast_build_stages(const char* expression) {
    // Count tokens
    Lexer* lexer = lexer_create(expression);
    size_t token_count = 0;
    while (lexer_get_next_token(lexer).type != TOKEN_EOF) token_count++;
    free(lexer);
    
    // Build ast stages list
    ASTNodeList *list = nodelist_create();
    for (size_t i = 0; i <= token_count; i++) {
        nodelist_append(list, ast_build_with_token_limit(expression, i));
    }
    
    return list;
}

double eval(const char* expression) {
    ASTNode* root = ast_build(expression);
    double result = ast_eval(root);
    ast_free(root);
    return result;
}