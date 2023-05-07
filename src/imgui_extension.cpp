#include "imgui_extension.hpp"

#ifndef TARGET_IS_BIGENDIAN
	#define _R_MASK 0xFF000000
	#define _G_MASK 0x00FF0000
	#define _B_MASK 0x0000FF00
	#define _A_MASK 0x000000FF

	#define _R_SHIFT 24
	#define _G_SHIFT 16
	#define _B_SHIFT 8
	#define _A_SHIFT 0
#else
	#define _R_MASK 0x000000FF
	#define _G_MASK 0x0000FF00
	#define _B_MASK 0x00FF0000
	#define _A_MASK 0xFF000000

	#define _R_SHIFT 0
	#define _G_SHIFT 8
	#define _B_SHIFT 16
	#define _A_SHIFT 24
#endif

#define U32_TO_RGBA(color)                            \
	ImVec4(                                           \
		(float)((color & _R_MASK) >> _R_SHIFT) / 255, \
		(float)((color & _G_MASK) >> _G_SHIFT) / 255, \
		(float)((color & _B_MASK) >> _B_SHIFT) / 255, \
		(float)((color & _A_MASK) >> _A_SHIFT) / 255  \
	)

IMGUI_API void ImGui::Ext_ToggleButton(const char* str_id, bool* v) {
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.6f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id)) /* && g.LastActiveIdTimer < ANIM_SPEED )*/ {
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered()) {
		col_bg = ImGui::GetColorU32(ImLerp(U32_TO_RGBA(0xC6C6C6FF), ImGui::GetStyle().Colors[ImGuiCol_Button], t));
	}
	else {
		col_bg = ImGui::GetColorU32(ImLerp(U32_TO_RGBA(0xD8D8D8FF), ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered], t));
	}

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

IMGUI_API bool ImGui::ColorEdit3(const char* label, uint8_t col[3], ImGuiColorEditFlags flags) {
	float col_flt[3] = { col[0]/255.0f, col[1]/255.0f, col[2]/255.0f };
	if (ImGui::ColorEdit3(label, col_flt, flags)) {
		col[0] = col_flt[0] * 255;
		col[1] = col_flt[1] * 255;
		col[2] = col_flt[2] * 255;
		return true;
	}
	return false;
}

IMGUI_API bool ImGui::ColorEdit4(const char* label, uint8_t col[4], ImGuiColorEditFlags flags) {
	float col_flt[4] = { col[0]/255.0f, col[1]/255.0f, col[2]/255.0f, col[3]/255.0f };
	if (ImGui::ColorEdit4(label, col_flt, flags)) {
		col[0] = col_flt[0] * 255;
		col[1] = col_flt[1] * 255;
		col[2] = col_flt[2] * 255;
		col[3] = col_flt[3] * 255;
		return true;
	}
	return false;
}
