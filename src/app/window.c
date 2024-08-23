#include "app/window.h"
#include "log/log.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "cimgui.h"
#include "cimgui_impl.h"

static GLFWwindow *window = NULL;
static float window_bg[3] = { 0, 0, 0 };

void _glfwErrCB(int error, const char *desc) {
	log_error("GLFW Error: %d - %s", error, desc);
}

int WindowCreate(const char *title, int width, int height, int resizable) {
	glfwInit();
	glfwSetErrorCallback(_glfwErrCB);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);

	window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (!window) {
		log_error("Failed to create GLFW window");
		window = NULL;
		return 1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		log_error("Failed to initialize GLAD");
		window = NULL;
		return 1;
	}

	glfwSwapInterval(0);

	igCreateContext(NULL);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	igStyleColorsDark(NULL);
	ImGuiIO* io = igGetIO();
	io->IniFilename = NULL;

	glfwShowWindow(window);

	return 0;
}

void WindowDestroy(void) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);
	glfwDestroyWindow(window);
	glfwTerminate();
	window = NULL;
}

void WindowSetIcon(int width, int height, unsigned char *pixels) {
	GLFWimage iconImage;
	iconImage.width = width;
	iconImage.height = height;
	iconImage.pixels = pixels;
	glfwSetWindowIcon(window, 1, &iconImage);
}

void WindowSetTitle(const char *title) {
	glfwSetWindowTitle(window, title);
}

void WindowSetBG(unsigned char r, unsigned char g, unsigned char b) {
	window_bg[0] = r / 255.0f;
	window_bg[1] = g / 255.0f;
	window_bg[2] = b / 255.0f;
}

int WindowShouldClose() {
	return glfwWindowShouldClose(window);
}

static double lastTime = 0, frameDelay = 0;

void WindowSetMaxFPS(int fps) {
	if (fps <= 0) {
		frameDelay = 0;
	} else {
		frameDelay = 1.f / fps;
	}
}

void WindowNewFrame() {
	lastTime = glfwGetTime();
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();
}

void WindowEndFrame() {
	igRender();

	glClearColor(window_bg[0], window_bg[1], window_bg[2], 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
	glfwSwapBuffers(window);

	while (glfwGetTime() < lastTime + frameDelay);
}

void WindowMinimize(void) {
	glfwIconifyWindow(window);
}

void WindowRestore(void) {
	glfwRestoreWindow(window);
}

void WindowMaximize(void) {
	glfwMaximizeWindow(window);
}

