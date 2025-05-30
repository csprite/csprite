#include "app/window.h"
#include "log/log.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "cimgui/cimgui_impl.h"

static GLFWwindow *window = NULL;
static float window_bg[3] = { 0, 0, 0 };

void _glfwErrCB(int error, const char *desc) {
	log_error("GLFW Error: %d - %s", error, desc);
}

void window_init(const char *title, int width, int height, int resizable) {
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

	if (window == NULL) {
		log_fatal("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		log_fatal("Failed to initialize GLAD");
	}

	glfwSwapInterval(0);

	igCreateContext(NULL);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	igStyleColorsDark(NULL);
	ImGuiIO* io = igGetIO();
	io->IniFilename = NULL;

	glfwShowWindow(window);
}

void window_deinit(void) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);
	glfwDestroyWindow(window);
	glfwTerminate();
	window = NULL;
}

void window_set_icon(int width, int height, unsigned char *pixels) {
	GLFWimage iconImage;
	iconImage.width = width;
	iconImage.height = height;
	iconImage.pixels = pixels;
	glfwSetWindowIcon(window, 1, &iconImage);
}

void window_set_title(const char *title) {
	glfwSetWindowTitle(window, title);
}

void window_set_bg(unsigned char r, unsigned char g, unsigned char b) {
	window_bg[0] = r / 255.0f;
	window_bg[1] = g / 255.0f;
	window_bg[2] = b / 255.0f;
}

int window_should_close() {
	return glfwWindowShouldClose(window);
}

static double lastTime = 0, frameDelay = 0;

void window_set_max_fps(int fps) {
	if (fps <= 0) {
		frameDelay = 0;
	} else {
		frameDelay = 1.f / fps;
	}
}

void window_new_frame() {
	lastTime = glfwGetTime();
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();
}

void window_end_frame() {
	igRender();

	glClearColor(window_bg[0], window_bg[1], window_bg[2], 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
	glfwSwapBuffers(window);

	while (glfwGetTime() < lastTime + frameDelay);
}

void window_minimize(void) {
	glfwIconifyWindow(window);
}

void window_restore(void) {
	glfwRestoreWindow(window);
}

void window_maximize(void) {
	glfwMaximizeWindow(window);
}

