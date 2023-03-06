#include "imgui.h"
#include "imgui_internal.h"
#include <cstdint>

// New ImGui Widgets Will Be Added Here, Still Need A Way To Style Them

namespace ImGui {
	IMGUI_API void Ext_ToggleButton(const char* str_id, bool* v); // Custom New Function From "imgui_toggle.cpp"

	// API with uint8_t instead of float
    IMGUI_API bool ColorEdit3(const char* label, uint8_t col[3], ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorEdit4(const char* label, uint8_t col[4], ImGuiColorEditFlags flags = 0);
}

