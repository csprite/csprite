#include "../include/GLFW/glfw3.h"
#include <string>

enum export_format_e { PNG, JPG };

void zoomAndLevelViewport(); // Zooms And Levels The Canvas.
void framebuffer_size_callback(GLFWwindow* window, int, int);
void process_input(GLFWwindow* window);
void window_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double, double);
void key_callback(GLFWwindow* window, int, int, int, int);
void viewport_set();
void adjust_zoom(bool increase);
unsigned char * get_char_data(unsigned char *data, int x, int y);
unsigned char * get_pixel(int x, int y);
void fill(int x, int y, unsigned char *old_colour);
void draw(int x, int y); // Draws on 1 x, y coordinate
void drawInBetween(int st_x, int st_y, int end_x, int end_y);
std::string FixFileExtension(std::string filepath);
void SaveImageFromCanvas(std::string filepath);
