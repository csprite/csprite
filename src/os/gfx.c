#include "os/gfx.h"
#include "base/types.h"

#include "os/linux/gfx.c"
#include "os/win32/gfx.c"

#include <GLFW/glfw3.h>
#include "log/log.h"

void _glfwErrCallback(int error, const char *desc) {
	log_error("GLFW Error: %d - %s", error, desc);
}

OS_Handle os_window_init(U64 width, U64 height, String8 title) {
	glfwInit();
	glfwSetErrorCallback(_glfwErrCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(width, height, (char*)title.str, NULL, NULL);

	if (window == NULL) {
		os_abort_with_message(1, str8_lit("Failed to create GLFW window!"));
	}

	OS_Handle w = {
		.value = (U64)window
	};
	return w;
}

void os_window_set_title(OS_Handle w, String8 title) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwSetWindowTitle(window, (char*)title.str);
}

B32 os_window_should_close(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	return glfwWindowShouldClose(window);
}

void os_window_show(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwShowWindow(window);
}

void os_window_release(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwDestroyWindow(window);
	glfwTerminate();
}
