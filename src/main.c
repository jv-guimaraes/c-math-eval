#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raymath.h>
#include "calc.h"

// Constants for visualization settings
#define MIN_ZOOM 0.1f
#define MAX_ZOOM 5.0f
#define ZOOM_SPEED 0.1f
#define NODE_RADIUS 30.0f
#define NODE_VERTICAL_SPACING 100.0f
#define NODE_HORIZONTAL_SPACING 80.0f
#define LINE_THICKNESS 2.0f
#define NODE_BORDER_THICKNESS 3.0f
#define NODE_SHADOW_OFFSET 5.0f
#define NODE_SHADOW_ALPHA 80
#define NULL_NODE_ALPHA 120
#define NULL_NODE_BORDER_THICKNESS 2.0f
#define SCREEN_WIDTH 880
#define SCREEN_HEIGHT 660
#define FONT_SIZE 20.0f
#define INFO_FONT_SIZE 20.0f
#define LARGE_FONT_SIZE 60
#define MAX_TEXT_BUFFER 64

// Font scaling factors
#define OPERATOR_FONT_SCALE 1.3f
#define FONT_PATH "resources/RobotoMono-Bold.ttf"

// Color definitions for different node types
static const Color NULL_NODE_COLOR = LIGHTGRAY;
static const Color NULL_NODE_BORDER_COLOR = GRAY;
static const Color NUMBER_NODE_COLOR = SKYBLUE;
static const Color BINARY_OP_NODE_COLOR = ORANGE;
static const Color UNARY_OP_NODE_COLOR = GOLD;
static const Color ERROR_NODE_COLOR = RED;

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

// Initialization and cleanup
static void initialize_visualization(VisualizationState* state, const char* expression);
static void cleanup_visualization(VisualizationState* state, ASTNodeList* astStages);

// Input handling
static void handle_input(VisualizationState* state);

// Drawing functions
static void draw_info_text(const VisualizationState* state, double result);
static void draw_node(ASTNode* node, ASTNode* rootNode, Vector2 position, float zoom, Font font, const char* expression, bool isRoot);
static void draw_node_connections(Vector2 position, Vector2 childPosition, float radius, float zoom);
static void draw_null_node(Vector2 position, float radius, float zoom, Font font, const char* expression, bool isRoot);
static void draw_expression_text(Vector2 position, float radius, float zoom, Font font, const char* expression);

// Node property handling
static NodeDrawProperties get_node_properties(const ASTNode* node, char* buffer, size_t bufferSize);

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AST Stages Visualizer");

    const char* expression = "1 + 2 * 3";
    VisualizationState state;
    initialize_visualization(&state, expression);

    ASTNodeList* astStages = ast_build_stages(expression);
    double result = ast_eval(ast_build(expression));

    state.totalStages = astStages ? astStages->size : 0;
    state.currentStageIndex = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        handle_input(&state);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            Vector2 drawRootPosition = Vector2Add(state.rootPosition, state.panOffset);

            if (astStages && state.currentStageIndex < state.totalStages) {
                ASTNode* currentStageNode = astStages->data[state.currentStageIndex];
                draw_node(currentStageNode, currentStageNode, drawRootPosition, 
                         state.zoomFactor, state.font, state.expression, true);
            } else {
                draw_null_node(drawRootPosition, state.zoomFactor, state.zoomFactor, 
                             state.font, state.expression, true);
            }

            draw_info_text(&state, result);
        }
        EndDrawing();
    }

    cleanup_visualization(&state, astStages);
    return 0;
}

static void initialize_visualization(VisualizationState* state, const char* expression) {
    *state = (VisualizationState){
        .rootPosition = (Vector2){ SCREEN_WIDTH / 2.0f, 100.0f },
        .panOffset = (Vector2){ 0 },
        .dragStartPosition = (Vector2){ 0 },
        .zoomFactor = 1.0f,
        .isDragging = false,
        .currentStageIndex = 0,
        .totalStages = 0,
        .expression = expression
    };

    state->font = LoadFontEx(FONT_PATH, LARGE_FONT_SIZE, 0, 250);
    if (state->font.texture.id == 0) {
        fprintf(stderr, "Failed to load font: %s\n", FONT_PATH);
        state->font = GetFontDefault();
    }
}

static void handle_input(VisualizationState* state) {
    // Handle zooming
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0) {
        state->zoomFactor = Clamp(state->zoomFactor + wheelMove * ZOOM_SPEED, MIN_ZOOM, MAX_ZOOM);
    }

    // Handle panning
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->isDragging = true;
        state->dragStartPosition = GetMousePosition();
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        state->isDragging = false;
    }

    if (state->isDragging) {
        Vector2 currentMousePosition = GetMousePosition();
        Vector2 dragDelta = Vector2Subtract(currentMousePosition, state->dragStartPosition);
        state->panOffset = Vector2Add(state->panOffset, dragDelta);
        state->dragStartPosition = currentMousePosition;
    }

    // Handle stage navigation
    if (IsKeyPressed(KEY_RIGHT) && state->currentStageIndex < state->totalStages - 1) {
        state->currentStageIndex++;
    }
    if (IsKeyPressed(KEY_LEFT) && state->currentStageIndex > 0) {
        state->currentStageIndex--;
    }
}

static void draw_info_text(const VisualizationState* state, double result) {
    const float lineSpacing = INFO_FONT_SIZE * 1.0f;
    float y = 10.0f;
    const char* instructions[] = {
        "Use mouse wheel to zoom",
        "Hold left mouse button to drag",
        "Use LEFT/RIGHT arrows to change stages",
        TextFormat("Zoom: %.2f", state->zoomFactor),
        TextFormat("Result: %.2f", result),
        TextFormat("Stage: %d/%d", state->currentStageIndex + 1, state->totalStages)
    };

    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
        Color textColor = i < 3 ? GRAY : DARKGRAY;
        DrawTextEx(state->font, instructions[i], (Vector2){10, y}, INFO_FONT_SIZE, 0, textColor);
        y += lineSpacing;
    }
}

static NodeDrawProperties get_node_properties(const ASTNode* node, char* buffer, size_t bufferSize) {
    NodeDrawProperties props = {
        .nodeColor = ERROR_NODE_COLOR,
        .textColor = BLACK,
        .text = "?",
        .fontSize = FONT_SIZE
    };

    if (!node) return props;

    switch (node->type) {
        case NODE_NUMBER:
            props.nodeColor = NUMBER_NODE_COLOR;
            snprintf(buffer, bufferSize, "%.2f", node->number);
            props.text = buffer;
            break;
        case NODE_BINARY_OP:
            props.nodeColor = BINARY_OP_NODE_COLOR;
            snprintf(buffer, bufferSize, "%c", node->binary.operator);
            props.text = buffer;
            props.fontSize *= OPERATOR_FONT_SCALE;
            break;
        case NODE_UNARY_OP:
            props.nodeColor = UNARY_OP_NODE_COLOR;
            snprintf(buffer, bufferSize, "%c", node->unary.operator);
            props.text = buffer;
            props.fontSize *= OPERATOR_FONT_SCALE;
            break;
    }

    return props;
}

static void draw_node_connections(Vector2 position, Vector2 childPosition, float radius, float zoom) {
    Vector2 direction = Vector2Subtract(childPosition, position);
    Vector2 normalized = Vector2Normalize(direction);
    Vector2 lineStart = Vector2Add(position, Vector2Scale(normalized, radius));
    DrawLineEx(lineStart, childPosition, LINE_THICKNESS * zoom, BLACK);
}

static void draw_expression_text(Vector2 position, float radius, float zoom, Font font, const char* expression) {
    if (!expression) return;

    Vector2 expressionTextSize = MeasureTextEx(font, expression, INFO_FONT_SIZE * zoom, 0);
    Vector2 expressionTextPosition = {
        position.x - expressionTextSize.x / 2,
        position.y - radius - expressionTextSize.y - 10 * zoom
    };
    DrawTextEx(font, expression, expressionTextPosition, INFO_FONT_SIZE * zoom, 0, BLACK);
}

static void draw_null_node(Vector2 position, float radius, float zoom, Font font, const char* expression, bool isRoot) {
    radius *= NODE_RADIUS;

    // Draw shadow
    DrawCircleV(
        (Vector2){position.x + NODE_SHADOW_OFFSET, position.y + NODE_SHADOW_OFFSET},
        radius, 
        (Color){0, 0, 0, NODE_SHADOW_ALPHA}
    );

    // Draw border
    DrawCircleV(position, radius + NULL_NODE_BORDER_THICKNESS, NULL_NODE_BORDER_COLOR);

    // Draw main node
    DrawCircleV(position, radius, Fade(NULL_NODE_COLOR, (float)NULL_NODE_ALPHA/255.0f));

    if (isRoot) {
        draw_expression_text(position, radius, zoom, font, expression);
    }
}

static void draw_node(ASTNode* node, ASTNode* rootNode, Vector2 position, float zoom, Font font, const char* expression, bool isRoot) {
    if (!node) {
        draw_null_node(position, zoom, zoom, font, expression, isRoot);
        return;
    }

    float radius = NODE_RADIUS * zoom;
    char buffer[MAX_TEXT_BUFFER];
    NodeDrawProperties props = get_node_properties(node, buffer, sizeof(buffer));
    props.fontSize *= zoom;

    // Draw node shadow
    DrawCircleV(
        (Vector2){position.x + NODE_SHADOW_OFFSET, position.y + NODE_SHADOW_OFFSET},
        radius, 
        (Color){0, 0, 0, NODE_SHADOW_ALPHA}
    );

    // Draw node border and main circle
    DrawCircleV(position, radius + NODE_BORDER_THICKNESS, Fade(DARKGRAY, 0.3f));
    DrawCircleV(position, radius, props.nodeColor);

    // Draw node text
    if (props.text) {
        Vector2 textSize = MeasureTextEx(font, props.text, props.fontSize, 0);
        Vector2 textPosition = {
            position.x - textSize.x/2,
            position.y - textSize.y/2 - (props.fontSize * 0.1f)
        };
        DrawTextEx(font, props.text, textPosition, props.fontSize, 0, props.textColor);
    }

    // Draw expression text for root node
    if (isRoot) {
        draw_expression_text(position, radius, zoom, font, expression);
    }

    // Draw children recursively
    if (node->type == NODE_BINARY_OP) {
        Vector2 leftPos = {
            position.x - NODE_HORIZONTAL_SPACING * zoom,
            position.y + NODE_VERTICAL_SPACING * zoom
        };
        Vector2 rightPos = {
            position.x + NODE_HORIZONTAL_SPACING * zoom,
            position.y + NODE_VERTICAL_SPACING * zoom
        };

        draw_node_connections(position, leftPos, radius, zoom);
        draw_node_connections(position, rightPos, radius, zoom);

        draw_node(node->binary.left, rootNode, leftPos, zoom, font, NULL, false);
        draw_node(node->binary.right, rootNode, rightPos, zoom, font, NULL, false);
    } else if (node->type == NODE_UNARY_OP) {
        Vector2 childPos = {
            position.x,
            position.y + NODE_VERTICAL_SPACING * zoom
        };

        draw_node_connections(position, childPos, radius, zoom);
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