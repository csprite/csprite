/*
	GLFW + OpenGL Backend, As For Now That's What We'll Be Using.
	But Later I'll Switch To Native APIs Depending On The OS.
*/

#include "app/app.hpp"
#include "assets.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static GLFWwindow* window = nullptr;

void* App::GetWindow() {
	return (void*)window;
}

i32 App::Init(u16 w, u16 h, const char* title) {
	glfwInit();
	// glfwSetErrorCallback(logGLFWErrors);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL; // Disable Generation of .ini file

	int Montserrat_Bold_Size = 0;
	const void* Montserrat_Bold = assets_get("data/fonts/Montserrat-Bold.ttf", &Montserrat_Bold_Size);
	io.Fonts->AddFontFromMemoryCompressedTTF(Montserrat_Bold, Montserrat_Bold_Size, 16.0f);

	return 0;
}

void App::NewFrame() {
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void App::EndFrame() {
	glClearColor(0.075, 0.075, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void App::Release() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
