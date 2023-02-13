#pragma once

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui {
	IMGUI_API void LoadStyleFrom(const char* fileName, ImGuiStyle& style);
	IMGUI_API void SaveStylesTo(const char* fileName);
}
