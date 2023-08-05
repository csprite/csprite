/*
	GLFW + OpenGL Backend, As For Now That's What We'll Be Using.
	But Later I'll Switch To Native APIs Depending On The OS.
*/

#include <chrono>
#include <thread>

#include "assets.h"

#include "app/app.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

static GLFWwindow* window = nullptr;

i32 App::Init(u16 w, u16 h, const char* title) {
	glfwInit();
	glfwSetErrorCallback([](int error, const char* desc) -> void {
		printf("Error: %d\n%s\n", error, desc);
	});
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	window = glfwCreateWindow(w, h, title, NULL, NULL);

	if (!window) {
		printf("Failed to create GLFW window\n");
		window = nullptr;
		return 1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to init GLAD\n");
		window = nullptr;
		return 1;
	}

	glfwSwapInterval(0);

	GLFWimage iconImage;
	iconImage.width = 32;
	iconImage.height = 32;
	iconImage.pixels = (u8*) assets_get("data/icons/icon-32.png", NULL);
	glfwSetWindowIcon(window, 1, &iconImage);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL; // Disable Generation of .ini file

	int Montserrat_Bold_Size = 0;
	const void* Montserrat_Bold = assets_get("data/fonts/Roboto-Regular.ttf", &Montserrat_Bold_Size);
	io.Fonts->AddFontFromMemoryCompressedTTF(Montserrat_Bold, Montserrat_Bold_Size, 16.0f);

	#define xstr(a) str(a)
	#define str(a) #a
	#define PRINT_GL_VER(ver) printf(xstr(ver) ": %d\n", ver)
	PRINT_GL_VER(GLAD_GL_VERSION_1_0);
	PRINT_GL_VER(GLAD_GL_VERSION_1_1);
	PRINT_GL_VER(GLAD_GL_VERSION_1_2);
	PRINT_GL_VER(GLAD_GL_VERSION_1_3);
	PRINT_GL_VER(GLAD_GL_VERSION_1_4);
	PRINT_GL_VER(GLAD_GL_VERSION_1_5);
	PRINT_GL_VER(GLAD_GL_VERSION_2_0);
	PRINT_GL_VER(GLAD_GL_VERSION_2_1);
	PRINT_GL_VER(GLAD_GL_VERSION_3_0);
	PRINT_GL_VER(GLAD_GL_VERSION_3_1);
	PRINT_GL_VER(GLAD_GL_VERSION_3_2);
	PRINT_GL_VER(GLAD_GL_VERSION_3_3);
	PRINT_GL_VER(GLAD_GL_VERSION_4_0);
	PRINT_GL_VER(GLAD_GL_VERSION_4_1);
	PRINT_GL_VER(GLAD_GL_VERSION_4_2);
	PRINT_GL_VER(GLAD_GL_VERSION_4_3);
	PRINT_GL_VER(GLAD_GL_VERSION_4_4);
	PRINT_GL_VER(GLAD_GL_VERSION_4_5);
	PRINT_GL_VER(GLAD_GL_VERSION_4_6);
	#undef PRINT_GL_VER
	#undef xstr
	#undef str

	printf("OpenGL version supported by this platform is %s\n", glGetString(GL_VERSION));

	glfwShowWindow(window);

	// triggers the updates to ImGui so display size and etc are set.
	App::NewFrame();
	App::EndFrame();

	return 0;
}

bool App::ShouldClose() {
	return glfwWindowShouldClose(window);
}

static f32 frameStart; // when frame started
static f32 frameTime; // how long that frame took
static f32 frameDelay = 1.0f / 59.0f;

void App::NewFrame() {
	frameStart = glfwGetTime();
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void App::EndFrame() {
	ImGui::Render();

	glClearColor(0.07, 0.07, 0.07, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);

	frameTime = glfwGetTime() - frameStart;
	if (frameDelay > frameTime) {
		std::this_thread::sleep_for(
			std::chrono::milliseconds((u32)((frameDelay - frameTime) * 1000))
		);
	}
	frameStart = glfwGetTime();
}

void App::Release() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void* App::GetWindow() {
	return (void*)window;
}

void App::SetTitle(const char* title) {
	if (title && window) {
		glfwSetWindowTitle(window, title);
	}
}
