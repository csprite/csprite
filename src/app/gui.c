#include "app/gui.h"
#include "log/log.h"
#include "assets/assets.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "cimgui/cimgui_impl.h"
#include "os/os.h"

void gui_init(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwMakeContextCurrent(window);
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		os_abort_with_message(1, str8_lit("Failed to initialize GLAD"));
	}

	glfwSwapInterval(0);

	igCreateContext(NULL);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO* io = igGetIO_Nil();
	io->IniFilename = NULL;

	const float UI_Scale = 1.0f;

	// Font
	S32 fontDataSize = 0;
	ImVector_ImWchar FontRanges;
	ImVector_ImWchar_Init(&FontRanges);
	ImFontGlyphRangesBuilder *FontBuilder = ImFontGlyphRangesBuilder_ImFontGlyphRangesBuilder();
	ImFontGlyphRangesBuilder_AddRanges(FontBuilder, ImFontAtlas_GetGlyphRangesDefault(io->Fonts));
	ImFontGlyphRangesBuilder_BuildRanges(FontBuilder, &FontRanges);
	ImFontAtlas_AddFontFromMemoryCompressedTTF(
	    io->Fonts, assets_get("data/fonts/Inter.ttf", &fontDataSize),
		fontDataSize, 18 * UI_Scale, NULL, FontRanges.Data
	);
	ImFontAtlas_Build(io->Fonts);
	if (!ImFontAtlas_IsBuilt(io->Fonts)) {
		log_fatal("io.Fonts->Build() - failed to build the font atlas");
	}
	ImFontGlyphRangesBuilder_destroy(FontBuilder);
	ImVector_ImWchar_UnInit(&FontRanges);

	// Colors & Scaling
	igStyleColorsDark(igGetStyle());
	ImGuiStyle_ScaleAllSizes(igGetStyle(), UI_Scale);

	gui_begin_frame(w);
	gui_end_frame(w);

	ImGuiStyle* style = igGetStyle();
	style->FrameRounding = igGetFontSize() * 0.25f;
}

void gui_release(OS_Handle window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);
}

void gui_begin_frame(OS_Handle window) {
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();
}

void gui_end_frame(OS_Handle w) {
	igEndFrame();
	igRender();

	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwSwapBuffers(window);
}
