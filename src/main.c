#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raymath.h>
#include "calc.h"

#define NODE_RADIUS 30.0f
#define FONT_SIZE 20.0f
#define INFO_FONT_SIZE 20.0f
#define OPERATOR_FONT_SCALE 1.3f
#define ZOOM_SPEED 0.1f
#define MIN_ZOOM 0.1f
#define MAX_ZOOM 5.0f
#define NODE_VERTICAL_SPACING 100.0f
#define NODE_HORIZONTAL_SPACING 80.0f
#define LINE_THICKNESS 2.0f
#define SCREEN_WIDTH 880
#define SCREEN_HEIGHT 660
#define FONT_PATH "resources/RobotoMono-Bold.ttf"
#define LARGE_FONT_SIZE 60
#define MAX_TEXT_BUFFER 64
#define NODE_BORDER_THICKNESS 3.0f
#define NODE_SHADOW_OFFSET 5.0f
#define NODE_SHADOW_ALPHA 80

typedef struct {
    Vector2 rootPosition;
    Vector2 panOffset;
    Vector2 dragStartPosition;
    float zoomFactor;
    bool isDragging;
    Font font;
} VisualizationState;

typedef struct {
    Color nodeColor;
    Color textColor;
    const char* text;
    float fontSize;
} NodeDrawProperties;

// Function prototypes
static void initialize_visualization(VisualizationState* state);
static void handle_input(VisualizationState* state);
static void draw_info_text(const VisualizationState* state, const char* expression, double result);
static NodeDrawProperties get_node_properties(const ASTNode* node, char* buffer, size_t bufferSize);
static void draw_node_connections(Vector2 position, Vector2 childPosition, float radius, float zoom);
static void draw_node(const ASTNode* node, Vector2 position, float zoom, Font font);
static void cleanup_visualization(VisualizationState* state, ASTNode* rootNode);

// int main(void) {
//     SetConfigFlags(FLAG_MSAA_4X_HINT);
//     InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AST Visualizer");

//     VisualizationState state;
//     initialize_visualization(&state);

//     const char* expression = "1";
//     ASTNode* rootNode = ast_build(expression);
//     double result = rootNode ? ast_eval(rootNode) : 0.0;

//     SetTargetFPS(60);

//     while (!WindowShouldClose()) {
//         handle_input(&state);

//         BeginDrawing();
//         {
//             ClearBackground(RAYWHITE);

//             // Calculate drawing position with pan offset
//             Vector2 drawRootPosition = Vector2Add(state.rootPosition, state.panOffset);

//             // Draw the AST only if rootNode exists.
//             if (rootNode) {
//                 draw_node(rootNode, drawRootPosition, state.zoomFactor, state.font);
//             }

//             draw_info_text(&state, expression, result);
//         }
//         EndDrawing();
//     }

//     cleanup_visualization(&state, rootNode);
//     return 0;
// }

static void initialize_visualization(VisualizationState* state) {
    // Initialize state with default values
    state->rootPosition = (Vector2){ SCREEN_WIDTH / 2.0f, 100.0f };
    state->panOffset = (Vector2){ 0.0f, 0.0f };
    state->dragStartPosition = (Vector2){ 0.0f, 0.0f };
    state->zoomFactor = 1.0f;
    state->isDragging = false;

    // Load font and handle potential loading failure
    state->font = LoadFontEx(FONT_PATH, LARGE_FONT_SIZE, 0, 250);
    if (state->font.texture.id == 0) {
        fprintf(stderr, "Failed to load font: %s\n", FONT_PATH);
        state->font = GetFontDefault(); // Fallback to default font
    }
}

static void handle_input(VisualizationState* state) {
    // Zooming with mouse wheel
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0) {
        state->zoomFactor = Clamp(state->zoomFactor + wheelMove * ZOOM_SPEED, MIN_ZOOM, MAX_ZOOM);
    }

    // Panning with mouse drag
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->isDragging = true;
        state->dragStartPosition = GetMousePosition();
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        state->isDragging = false;
    }
    if (state->isDragging) {
        Vector2 currentMousePosition = GetMousePosition();
        Vector2 dragDelta = Vector2Subtract(currentMousePosition, state->dragStartPosition);
        state->panOffset = Vector2Add(state->panOffset, dragDelta);
        state->dragStartPosition = currentMousePosition;  // Update drag start for next frame
    }
}

static void draw_info_text(const VisualizationState* state, const char* expression, double result) {
    float y = 10.0f;
    const float lineSpacing = INFO_FONT_SIZE * 1.0f;

    // Display various information: instructions, zoom level, expression, result
    DrawTextEx(state->font, "Use mouse wheel to zoom", (Vector2){10, y}, INFO_FONT_SIZE, 0, GRAY);
    y += lineSpacing;
    DrawTextEx(state->font, "Hold left mouse button to drag", (Vector2){10, y}, INFO_FONT_SIZE, 0, GRAY);
    y += lineSpacing;
    DrawTextEx(state->font, TextFormat("Zoom: %.2f", state->zoomFactor), (Vector2){10, y}, INFO_FONT_SIZE, 0, DARKGRAY);
    y += lineSpacing;
    DrawTextEx(state->font, TextFormat("%s = %.2f", expression, result), (Vector2){10, y}, INFO_FONT_SIZE, 0, DARKGRAY);
}

// Determine node properties based on its type (number, binary operator, unary operator)
static NodeDrawProperties get_node_properties(const ASTNode* node, char* buffer, size_t bufferSize) {
    NodeDrawProperties props = {
        .nodeColor = LIGHTGRAY,
        .textColor = BLACK,
        .text = NULL,
        .fontSize = FONT_SIZE
    };

    // Switch on node type to set appropriate properties
    switch (node->type) {
        case NODE_NUMBER:
            props.nodeColor = SKYBLUE;
            snprintf(buffer, bufferSize, "%.2f", node->number);
            props.text = buffer;
            break;
        case NODE_BINARY_OP:
            props.nodeColor = ORANGE;
            snprintf(buffer, bufferSize, "%c", node->binary.operator);
            props.text = buffer;
            props.fontSize *= OPERATOR_FONT_SCALE; // Increase font size for operators
            break;
        case NODE_UNARY_OP:
            props.nodeColor = GOLD;
            snprintf(buffer, bufferSize, "%c", node->unary.operator);
            props.text = buffer;
            props.fontSize *= OPERATOR_FONT_SCALE;
            break;
        default:
            props.nodeColor = RED;
            props.text = "?"; // Indicate an unknown node type
            break;
    }

    return props;
}

// Draw a connection line between a parent and child node
static void draw_node_connections(Vector2 position, Vector2 childPosition, float radius, float zoom) {
    Vector2 direction = Vector2Subtract(childPosition, position);
    Vector2 normalized = Vector2Normalize(direction);
    Vector2 lineStart = Vector2Add(position, Vector2Scale(normalized, radius));

    DrawLineEx(lineStart, childPosition, LINE_THICKNESS * zoom, BLACK);
}


// Recursively draw the AST node and its children
static void draw_node(const ASTNode* node, Vector2 position, float zoom, Font font) {
    if (!node) return;

    float radius = NODE_RADIUS * zoom;
    char buffer[MAX_TEXT_BUFFER];
    NodeDrawProperties props = get_node_properties(node, buffer, sizeof(buffer));
    props.fontSize *= zoom;

    // Shadow effect
    DrawCircleV((Vector2){position.x + NODE_SHADOW_OFFSET, position.y + NODE_SHADOW_OFFSET}, 
                radius, (Color){0, 0, 0, NODE_SHADOW_ALPHA});

    // Node border
    DrawCircleV(position, radius + NODE_BORDER_THICKNESS, Fade(DARKGRAY, 0.3f));
    
    // Main node
    DrawCircleV(position, radius, props.nodeColor);

    // Text rendering
    if (props.text) {
        Vector2 textSize = MeasureTextEx(font, props.text, props.fontSize, 0);
        Vector2 textPosition = {
            position.x - textSize.x/2,
            position.y - textSize.y/2 - (props.fontSize * 0.1f) // Vertical centering adjustment
        };
        DrawTextEx(font, props.text, textPosition, props.fontSize, 0, props.textColor);
    }

    // Recursively draw children based on node type
    if (node->type == NODE_BINARY_OP) {
        // Calculate positions for left and right children
        Vector2 leftPos = {
            position.x - NODE_HORIZONTAL_SPACING * zoom,
            position.y + NODE_VERTICAL_SPACING * zoom
        };
        Vector2 rightPos = {
            position.x + NODE_HORIZONTAL_SPACING * zoom,
            position.y + NODE_VERTICAL_SPACING * zoom
        };

        // Draw connecting lines
        draw_node_connections(position, leftPos, radius, zoom);
        draw_node_connections(position, rightPos, radius, zoom);

        // Recursively draw left and right subtrees
        draw_node(node->binary.left, leftPos, zoom, font);
        draw_node(node->binary.right, rightPos, zoom, font);
    } else if (node->type == NODE_UNARY_OP) {
        // Calculate position for the single child
        Vector2 childPos = {
            position.x,
            position.y + NODE_VERTICAL_SPACING * zoom
        };

        // Draw connecting line
        draw_node_connections(position, childPos, radius, zoom);
         // Recursively draw the child subtree
        draw_node(node->unary.operand, childPos, zoom, font);
    }
}

static void cleanup_visualization(VisualizationState* state, ASTNode* rootNode) {
    if (rootNode) {
        ast_free(rootNode);
    }
    UnloadFont(state->font);
    CloseWindow();
}
