#include "../include/GLFW/glfw3.h"
#include <string>

void openUrl(std::string url);
void logGLFWErrors(int error, const char *description); // GLFW Error Callback
void zoomAndLevelViewport(); // Zooms And Levels The Canvas.
void framebuffer_size_callback(GLFWwindow* window, int, int);
void process_input(GLFWwindow* window);
void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double, double);
void mouse_button_callback(GLFWwindow* window, int, int, int);
void scroll_callback(GLFWwindow* window, double, double);
void key_callback(GLFWwindow* window, int, int, int, int);
void viewport_set();
void adjust_zoom(bool increase);
int string_to_int(int *out, char *s);
int color_equal(unsigned char *, unsigned char *);
unsigned char * get_pixel(int x, int y);
void fill(int x, int y, unsigned char *old_colour);
void draw(int x, int y);
void load_image_to_canvas();
void save_image_from_canvas();
unsigned char * get_char_data(unsigned char *data, int x, int y);
