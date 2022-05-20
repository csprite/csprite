#ifndef MAIN_H
#define MAIN_H

#include "../include/GLFW/glfw3.h"
#include <string>

enum export_format_e { PNG, JPG };

// Callbacks
void process_input(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double, double);
void key_callback(GLFWwindow* window, int, int, int, int);
void framebuffer_size_callback(GLFWwindow* window, int, int);
void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Canvas Zoom
void viewport_set();
void zoomAndLevelViewport(); // Zooms And Levels The Canvas.
void adjust_zoom(bool increase);

// Little Helpers
unsigned char * get_char_data(unsigned char *data, int x, int y);
unsigned char * get_pixel(int x, int y);

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
int FreeHistory();
void ResetHistory();

#endif // end MAIN_H
