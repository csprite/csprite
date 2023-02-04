#include <stdio.h>

#include "imgui_styles.h"
#include "ini/ini.h"

extern "C" int strncmpci(const char* s1, const char* s2, size_t n); // src/utils.h - Case insensitive string compare upto n

static const char* ImGuiDirToText(int direction) {
	switch (direction) {
		case ImGuiDir_None:   return "None";
		case ImGuiDir_Left:   return "Left";
		case ImGuiDir_Right:  return "Right";
		case ImGuiDir_Up:     return "Up";
		case ImGuiDir_Down:   return "Down";
		default:              return "Invalid";
	}
}

static int ImGuiTextToDir(const char* direction) {
	if (direction == NULL) return -1;
	if (strncmpci(direction, "None", 4) == 0)       return ImGuiDir_None;
	else if (strncmpci(direction, "Left", 4) == 0)  return ImGuiDir_Left;
	else if (strncmpci(direction, "Right", 5) == 0) return ImGuiDir_Right;
	else if (strncmpci(direction, "Up", 2) == 0)    return ImGuiDir_Up;
	else if (strncmpci(direction, "Down", 4) == 0)  return ImGuiDir_Down;
	else return -1;
}

IMGUI_API void ImGui::SaveStylesTo(const char* fileName) {
	ImGuiStyle& style = ImGui::GetStyle();
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL) return;

	// Header
	fprintf(fp, "[ImGuiStyles]\n");

	#define WRITE_FLOAT(name) fprintf(fp, #name" = %f\n", style.name)
	#define WRITE_ImVec2(name) fprintf(fp, #name" = %f,%f\n", style.name.x, style.name.y);
	#define WRITE_DIRECTION(name) fprintf(fp, #name" = %s\n", ImGuiDirToText(style.name));
	#define WRITE_BOOLEAN(name) fprintf(fp, #name" = %s\n", style.name == true ? "true" : "false");

	// Floats
	WRITE_FLOAT(Alpha);
	WRITE_FLOAT(DisabledAlpha);
	WRITE_FLOAT(WindowRounding);
	WRITE_FLOAT(WindowBorderSize);
	WRITE_FLOAT(ChildRounding);
	WRITE_FLOAT(ChildBorderSize);
	WRITE_FLOAT(PopupRounding);
	WRITE_FLOAT(PopupBorderSize);
	WRITE_FLOAT(FrameRounding);
	WRITE_FLOAT(FrameBorderSize);
	WRITE_FLOAT(IndentSpacing);
	WRITE_FLOAT(ColumnsMinSpacing);
	WRITE_FLOAT(ScrollbarSize);
	WRITE_FLOAT(ScrollbarRounding);
	WRITE_FLOAT(GrabMinSize);
	WRITE_FLOAT(GrabRounding);
	WRITE_FLOAT(LogSliderDeadzone);
	WRITE_FLOAT(TabRounding);
	WRITE_FLOAT(TabBorderSize);
	WRITE_FLOAT(TabMinWidthForCloseButton);
	WRITE_FLOAT(MouseCursorScale);
	WRITE_FLOAT(CurveTessellationTol);
	WRITE_FLOAT(CircleTessellationMaxError);

	// Directions
	WRITE_DIRECTION(WindowMenuButtonPosition);
	WRITE_DIRECTION(ColorButtonPosition);

	// Booleans
	WRITE_BOOLEAN(AntiAliasedLines);
	WRITE_BOOLEAN(AntiAliasedLinesUseTex);
	WRITE_BOOLEAN(AntiAliasedFill);

	// ImVec2s
	WRITE_ImVec2(WindowPadding);
	WRITE_ImVec2(WindowMinSize);
	WRITE_ImVec2(WindowTitleAlign);
	WRITE_ImVec2(FramePadding);
	WRITE_ImVec2(ItemSpacing);
	WRITE_ImVec2(ItemInnerSpacing);
	WRITE_ImVec2(CellPadding);
	WRITE_ImVec2(TouchExtraPadding);
	WRITE_ImVec2(ButtonTextAlign);
	WRITE_ImVec2(SelectableTextAlign);
	WRITE_ImVec2(DisplayWindowPadding);
	WRITE_ImVec2(DisplaySafeAreaPadding);

	fprintf(fp, "\n[ImGuiColors]\n");
	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		const char* name = ImGui::GetStyleColorName(i);
		int color[4] = {
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].x), 0, 255),
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].y), 0, 255),
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].z), 0, 255),
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].w), 0, 255)
		};
		fprintf(fp, "%s = #%02X%02X%02X%02X\n", name, color[0], color[1], color[2], color[3]);
	}

	fclose(fp);
	fp = NULL;
}


IMGUI_API void ImGui::LoadStyleFrom(const char* fileName, ImGuiStyle& style) {
	ini_t* ini_style = ini_load(fileName);
	if (ini_style == NULL) return;
	// if (style == NULL) return;

	#define LOAD_FLOAT(name) { \
		const char* Str_##name  = ini_get(ini_style, "ImGuiStyles", #name); \
		style.name = Str_##name != NULL ? atof(Str_##name) : style.name; \
	}

	// Floats
	LOAD_FLOAT(Alpha);
	LOAD_FLOAT(DisabledAlpha);
	LOAD_FLOAT(WindowRounding);
	LOAD_FLOAT(WindowBorderSize);
	LOAD_FLOAT(ChildRounding);
	LOAD_FLOAT(ChildBorderSize);
	LOAD_FLOAT(PopupRounding);
	LOAD_FLOAT(PopupBorderSize);
	LOAD_FLOAT(FrameRounding);
	LOAD_FLOAT(FrameBorderSize);
	LOAD_FLOAT(IndentSpacing);
	LOAD_FLOAT(ColumnsMinSpacing);
	LOAD_FLOAT(ScrollbarSize);
	LOAD_FLOAT(ScrollbarRounding);
	LOAD_FLOAT(GrabMinSize);
	LOAD_FLOAT(GrabRounding);
	LOAD_FLOAT(LogSliderDeadzone);
	LOAD_FLOAT(TabRounding);
	LOAD_FLOAT(TabBorderSize);
	LOAD_FLOAT(TabMinWidthForCloseButton);
	LOAD_FLOAT(MouseCursorScale);
	LOAD_FLOAT(CurveTessellationTol);
	LOAD_FLOAT(CircleTessellationMaxError);

	// Directions
	#define LOAD_DIRECTION(name) { \
		const char* str  = ini_get(ini_style, "ImGuiStyles", #name); \
		if (str != NULL) { \
			int direction = ImGuiTextToDir(str); \
			if (direction >= 0) style.name = direction; \
		} \
	}
	LOAD_DIRECTION(WindowMenuButtonPosition);
	LOAD_DIRECTION(ColorButtonPosition);

	// Booleans
	#define LOAD_BOOLEANS(name) { \
		const char* str  = ini_get(ini_style, "ImGuiStyles", #name); \
		if (str != NULL) { \
			if (strncmpci(str, "true", 4) == 0) style.name = true; \
			else if (strncmpci(str, "false", 5) == 0) style.name = false; \
		} \
	}
	LOAD_BOOLEANS(AntiAliasedLines);
	LOAD_BOOLEANS(AntiAliasedLinesUseTex);
	LOAD_BOOLEANS(AntiAliasedFill);

	#define MAX_BEFORE_COMMA_SIZE 512
	char BeforeComma[MAX_BEFORE_COMMA_SIZE] = "";

	#define LOAD_IMVEC2s(var_imvec2, name) { \
		const char* str = ini_get(ini_style, "ImGuiStyles", #name); \
		const char* CommaOffset = str == NULL ? NULL : strchr(str, ','); \
		if (CommaOffset != NULL && CommaOffset - str > 0) { \
			strncpy(BeforeComma, str, CommaOffset - str); \
			BeforeComma[(CommaOffset - str) + 1] = '\0'; \
			var_imvec2.x = atof(BeforeComma); \
			var_imvec2.y = atof(CommaOffset + 1); \
		} \
		BeforeComma[0] = '\0'; \
	}

	// ImVec2s
	LOAD_IMVEC2s(style.WindowPadding, WindowPadding);
	LOAD_IMVEC2s(style.WindowMinSize, WindowMinSize);
	LOAD_IMVEC2s(style.WindowTitleAlign, WindowTitleAlign);
	LOAD_IMVEC2s(style.FramePadding, FramePadding);
	LOAD_IMVEC2s(style.ItemSpacing, ItemSpacing);
	LOAD_IMVEC2s(style.ItemInnerSpacing, ItemInnerSpacing);
	LOAD_IMVEC2s(style.CellPadding, CellPadding);
	LOAD_IMVEC2s(style.TouchExtraPadding, TouchExtraPadding);
	LOAD_IMVEC2s(style.ButtonTextAlign, ButtonTextAlign);
	LOAD_IMVEC2s(style.SelectableTextAlign, SelectableTextAlign);
	LOAD_IMVEC2s(style.DisplayWindowPadding, DisplayWindowPadding);
	LOAD_IMVEC2s(style.DisplaySafeAreaPadding, DisplaySafeAreaPadding);

	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		const char* name = ImGui::GetStyleColorName(i);
		const char* value = ini_get(ini_style, "ImGuiColors", name);
		if (value != NULL) {
			unsigned int color[4] = { 0, 0, 0, 0 };
			if (sscanf(value[0] == '#' ? value + 1 : value, "%02x%02x%02x%02x", &color[0], &color[1], &color[2], &color[3]) == 4) {
				style.Colors[i].x = (float)color[0] / 255;
				style.Colors[i].y = (float)color[1] / 255;
				style.Colors[i].z = (float)color[2] / 255;
				style.Colors[i].w = (float)color[3] / 255;
			}
		}
	}

	ini_free(ini_style);
	ini_style = NULL;
}
