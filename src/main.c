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
#define NULL_NODE_COLOR LIGHTGRAY
#define NULL_NODE_BORDER_COLOR GRAY
#define NULL_NODE_BORDER_THICKNESS 2.0f
#define NULL_NODE_ALPHA 120

typedef struct {
    Vector2 rootPosition;
    Vector2 panOffset;
    Vector2 dragStartPosition;
    float zoomFactor;
    bool isDragging;
    Font font;
    int currentStageIndex;
    int totalStages;
    const char* expression;
} VisualizationState;

typedef struct {
    Color nodeColor;
    Color textColor;
    const char* text;
    float fontSize;
} NodeDrawProperties;

// Function prototypes
static void initialize_visualization(VisualizationState* state, const char* expression);
static void handle_input(VisualizationState* state);
static void draw_info_text(const VisualizationState* state, double result);
static NodeDrawProperties get_node_properties(const ASTNode* node, char* buffer, size_t bufferSize);
static void draw_node_connections(Vector2 position, Vector2 childPosition, float radius, float zoom);
static void draw_node(ASTNode* node, ASTNode* rootNode, Vector2 position, float zoom, Font font, const char* expression, bool isRoot);
static void cleanup_visualization(VisualizationState* state, ASTNodeList* astStages);

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AST Stages Visualizer");

    VisualizationState state;
    const char* expression = "1 + 2 * 3";
    initialize_visualization(&state, expression);
    state.expression = expression;

    ASTNodeList* astStages = ast_build_stages(expression);
    double result = ast_eval(ast_build(expression));

    state.totalStages = astStages ? astStages->size : 0;
    state.currentStageIndex = 0; // Start at the first stage

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        handle_input(&state);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            // Draw only the current stage AST if stages are available
            if (astStages && state.currentStageIndex < state.totalStages) {
                ASTNode* currentStageNode = astStages->data[state.currentStageIndex];
                // Calculate drawing position with pan offset
                Vector2 drawRootPosition = Vector2Add(state.rootPosition, state.panOffset);
                draw_node(currentStageNode, currentStageNode, drawRootPosition, state.zoomFactor, state.font, state.expression, true);
            } else {
                // Draw NULL node representation for empty tree, still root for expression
                Vector2 drawRootPosition = Vector2Add(state.rootPosition, state.panOffset);
                draw_node(NULL, NULL, drawRootPosition, state.zoomFactor, state.font, state.expression, true);
            }

            draw_info_text(&state, result);
        }
        EndDrawing();
    }

    cleanup_visualization(&state, astStages);
    return 0;
}

static void initialize_visualization(VisualizationState* state, const char* expression) { 
    // Initialize state with default values
    state->rootPosition = (Vector2){ SCREEN_WIDTH / 2.0f, 100.0f };
    state->panOffset = (Vector2){ 0.0f, 0.0f };
    state->dragStartPosition = (Vector2){ 0.0f, 0.0f };
    state->zoomFactor = 1.0f;
    state->isDragging = false;
    state->currentStageIndex = 0;
    state->totalStages = 0;
    state->expression = expression;


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

    // Stage cycling with keyboard, stop at boundaries
    if (IsKeyPressed(KEY_RIGHT)) {
        if (state->currentStageIndex < state->totalStages - 1) {
            state->currentStageIndex++;
        }
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (state->currentStageIndex > 0) {
            state->currentStageIndex--;
        }
    }
}

static void draw_info_text(const VisualizationState* state, double result) { // Expression removed here
    float y = 10.0f;
    const float lineSpacing = INFO_FONT_SIZE * 1.0f;

    // Display various information: instructions, zoom level, expression, result
    DrawTextEx(state->font, "Use mouse wheel to zoom", (Vector2){10, y}, INFO_FONT_SIZE, 0, GRAY);
    y += lineSpacing;
    DrawTextEx(state->font, "Hold left mouse button to drag", (Vector2){10, y}, INFO_FONT_SIZE, 0, GRAY);
    y += lineSpacing;
    DrawTextEx(state->font, "Use LEFT/RIGHT arrows to change stages", (Vector2){10, y}, INFO_FONT_SIZE, 0, GRAY);
    y += lineSpacing;
    DrawTextEx(state->font, TextFormat("Zoom: %.2f", state->zoomFactor), (Vector2){10, y}, INFO_FONT_SIZE, 0, DARKGRAY);
    y += lineSpacing;
    DrawTextEx(state->font, TextFormat("Result: %.2f", result), (Vector2){10, y}, INFO_FONT_SIZE, 0, DARKGRAY);
    y += lineSpacing;
    DrawTextEx(state->font, TextFormat("Stage: %d/%d", state->currentStageIndex + 1, state->totalStages), (Vector2){10, y}, INFO_FONT_SIZE, 0, DARKGRAY);
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
            props.text = "?";
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
static void draw_node(ASTNode* node, ASTNode* rootNode, Vector2 position, float zoom, Font font, const char* expression, bool isRoot) { // Root node and expression passed, isRoot added
    if (node == NULL) {
        float radius = NODE_RADIUS * zoom;

        // Shadow effect for NULL node
        DrawCircleV((Vector2){position.x + NODE_SHADOW_OFFSET, position.y + NODE_SHADOW_OFFSET},
                    radius, (Color){0, 0, 0, NODE_SHADOW_ALPHA});

        // NULL Node border
        DrawCircleV(position, radius + NULL_NODE_BORDER_THICKNESS, NULL_NODE_BORDER_COLOR);

        // Main NULL node - transparent circle
        DrawCircleV(position, radius, Fade(NULL_NODE_COLOR, (float)NULL_NODE_ALPHA/255.0f)); // Semi-transparent light gray

        // Draw expression above the NULL "root" node if it's the root call
        if (isRoot && expression != NULL) {
            Vector2 expressionTextSize = MeasureTextEx(font, expression, INFO_FONT_SIZE * zoom, 0);
            Vector2 expressionTextPosition = {
                position.x - expressionTextSize.x / 2,
                position.y - radius - expressionTextSize.y - 10 * zoom // Position above the node
            };
            DrawTextEx(font, expression, expressionTextPosition, INFO_FONT_SIZE * zoom, 0, BLACK);
        }

        return; // Stop drawing children for NULL node
    }

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

    // Text rendering inside node
    if (props.text) {
        Vector2 textSize = MeasureTextEx(font, props.text, props.fontSize, 0);
        Vector2 textPosition = {
            position.x - textSize.x/2,
            position.y - textSize.y/2 - (props.fontSize * 0.1f) // Vertical centering adjustment
        };
        DrawTextEx(font, props.text, textPosition, props.fontSize, 0, props.textColor);
    }

    // Draw expression above the root node
    if (isRoot && expression != NULL) { // Check if it's root call
        Vector2 expressionTextSize = MeasureTextEx(font, expression, INFO_FONT_SIZE * zoom, 0);
        Vector2 expressionTextPosition = {
            position.x - expressionTextSize.x / 2,
            position.y - radius - expressionTextSize.y - 10 * zoom // Position above the node
        };
        DrawTextEx(font, expression, expressionTextPosition, INFO_FONT_SIZE * zoom, 0, BLACK);
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

        // Recursively draw left and right subtrees, isRoot=false for children
        draw_node(node->binary.left, rootNode, leftPos, zoom, font, NULL, false);
        draw_node(node->binary.right, rootNode, rightPos, zoom, font, NULL, false);
    } else if (node->type == NODE_UNARY_OP) {
        // Calculate position for the single child
        Vector2 childPos = {
            position.x,
            position.y + NODE_VERTICAL_SPACING * zoom
        };

        // Draw connecting line
        draw_node_connections(position, childPos, radius, zoom);
         // Recursively draw the child subtree, isRoot=false for children
        draw_node(node->unary.operand, rootNode, childPos, zoom, font, NULL, false);
    }
}

static void cleanup_visualization(VisualizationState* state, ASTNodeList* astStages) {
    if (astStages) {
        nodelist_free(astStages);
    }
    UnloadFont(state->font);
    CloseWindow();
}