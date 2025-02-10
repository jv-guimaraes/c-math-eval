# AST Visualizer

This program visualizes the Abstract Syntax Tree (AST) of a mathematical expression using Raylib.  It takes a mathematical expression as input, builds the AST, and then renders the tree in a graphical window, allowing you to zoom and pan.

## Compilation:

To compile it, use the following command:

```bash
gcc -Iinclude -Llib src/*.c -o build/calc.exe -lraylib -lopengl32 -lgdi32 -lwinmm