# AST Visualizer

This program visualizes the Abstract Syntax Tree (AST) of a mathematical expression using Raylib.  It takes a mathematical expression as input, builds the AST, and then renders the tree in a graphical window, allowing you to zoom and pan.
![calc_S2T0RN27i2](https://github.com/user-attachments/assets/bbe2c191-f8e9-4f49-8cce-932842906df1)


## Compilation:

To compile the project, you'll typically use a command similar to the following (adjust paths as needed):

```bash
gcc -Iinclude -Llib src/*.c -o calc -lraylib -lopengl32 -lgdi32 -lwinmm
