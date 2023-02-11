#include "imgui.h"
#include "imgui_internal.h"

// New ImGui Widgets Will Be Added Here, Still Need A Way To Style Them

namespace ImGui {
	IMGUI_API void Ext_ToggleButton(const char* str_id, bool* v); // Custom New Function From "imgui_toggle.cpp"
	IMGUI_API void ShowDemoWindow(bool*);
	IMGUI_API void ShowAboutWindow(bool*);
	IMGUI_API void ShowStyleEditor(ImGuiStyle*);
	IMGUI_API bool ShowStyleSelector(char const*);
	IMGUI_API void ShowFontSelector(char const*);
	IMGUI_API void ShowUserGuide();
}

