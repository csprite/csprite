#include "GLFW/glfw3.h"
#include <string>

void framebuffer_size_callback(GLFWwindow *, int, int);
void process_input(GLFWwindow *);
void mouse_callback(GLFWwindow *, double, double);
void mouse_button_callback(GLFWwindow *, int, int, int);
void scroll_callback(GLFWwindow *, double, double);
void key_callback(GLFWwindow *, int, int, int, int);
void viewport_set();
void adjust_zoom(int);
int string_to_int(int *out, char *s);
int color_equal(unsigned char *, unsigned char *);
unsigned char * get_pixel(int x, int y);
void fill(int x, int y, unsigned char *old_colour);
void draw(int x, int y);
void draw_size2(int x, int y);
void draw_size3(int x, int y);
unsigned char * get_char_data(unsigned char *data, int x, int y);