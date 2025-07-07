#include "app/gui.h"
#include "app/imgui.h"
#include "app/render.h"
#include "log/log.h"
#include "assets/assets.h"

#ifdef TARGET_WINDOWS
	#include <windows.h>
	#include <GL/GL.h>
#else
	#include "glad/glad.h"
#endif
#include "cimgui/cimgui_impl.h"

void gui_init(OS_Handle w) {
	igCreateContext(NULL);
#ifdef TARGET_WINDOWS
	ImGui_ImplWin32_InitForOpenGL(os_window_get_native_handle(w));
#else
	ImGui_ImplGlfw_InitForOpenGL(os_window_get_native_handle(w), true);
#endif
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

	// https://github.com/Patitotective/ImThemes
	ImGuiStyle* style = igGetStyle();
	style->Alpha = 1.0f;
	style->DisabledAlpha = 0.5f;
	style->WindowPadding = (ImVec2){ 10.0f, 10.0f };
	style->WindowRounding = 0;
	style->WindowBorderSize = 1.0f;
	style->WindowMinSize = (ImVec2){ 20.0f, 20.0f };
	style->WindowTitleAlign = (ImVec2){ 0.0f, 0.5f };
	style->WindowMenuButtonPosition = ImGuiDir_Left;
	style->ChildRounding = 5.0f;
	style->ChildBorderSize = 1.0f;
	style->PopupRounding = 5.0f;
	style->PopupBorderSize = 1.0f;
	style->FramePadding = (ImVec2){ 5.0f, 5.0f };
	style->FrameRounding = 5.0f;
	style->FrameBorderSize = 0.0f;
	style->ItemSpacing = (ImVec2){ 8.0f, 5.0f };
	style->ItemInnerSpacing = (ImVec2){ 5.0f, 5.0f };
	style->CellPadding = (ImVec2){ 5.0f, 5.0f };
	style->IndentSpacing = 10.0f;
	style->ColumnsMinSpacing = 5.0f;
	style->ScrollbarSize = 10.0f;
	style->ScrollbarRounding = 5.0f;
	style->GrabMinSize = 10.0f;
	style->GrabRounding = 0.0f;
	style->TabRounding = 10.0f;
	style->TabBorderSize = 0.0f;
	style->ColorButtonPosition = ImGuiDir_Right;
	style->ButtonTextAlign = (ImVec2){ 0.5f, 0.5f };
	style->SelectableTextAlign = (ImVec2){ 0.0f, 0.0f };

	ImGuiStyle_ScaleAllSizes(style, 1);
}

void gui_release(OS_Handle window) {
	ImGui_ImplOpenGL3_Shutdown();
#ifdef TARGET_WINDOWS
	ImGui_ImplWin32_Shutdown();
#else
	ImGui_ImplGlfw_Shutdown();
#endif
	igDestroyContext(NULL);
	NoOp(window);
}

void gui_begin_frame(OS_Handle window) {
	os_window_poll_events(window);
	ImGui_ImplOpenGL3_NewFrame();
#ifdef TARGET_WINDOWS
	ImGui_ImplWin32_NewFrame();
#else
	ImGui_ImplGlfw_NewFrame();
#endif
	igNewFrame();
}

void gui_end_frame(OS_Handle w) {
	igEndFrame();
	igRender();
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
	os_window_swap(w);
}
