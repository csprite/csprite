#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "GLFW/glfw3.h"

enum export_format_e { PNG, JPG };

// Callbacks
void ProcessInput(GLFWwindow* window);
void ScrollCallback(GLFWwindow* window, double, double);
void KeyCallback(GLFWwindow* window, int, int, int, int);
void FrameBufferSizeCallback(GLFWwindow* window, int, int);
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// Canvas Zoom
void ViewportSet();
void ZoomNLevelViewport(); // Zooms And Levels The Canvas.
void AdjustZoom(bool increase);

// Little Helpers
unsigned char * GetPixel(int x, int y);

// Canvas Operations
void fill(int x, int y, unsigned char *old_colour);
void draw(int x, int y); // Draws on 1 x, y coordinate
void drawInBetween(int st_x, int st_y, int end_x, int end_y); // Draws Between The Given 2 x, y coordinates

// File IO
std::string FixFileExtension(std::string filepath);
void SaveImageFromCanvas(std::string filepath);

// Undo/Redo Related
int Undo();
int Redo();
void SaveState();
void FreeHistory();

#endif // end MAIN_H
