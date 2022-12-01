#include "imgui_extension.h"

#define U32_TO_RGBA(color)                         \
	ImVec4(                                        \
		(float)((color & 0xff000000) >> 24) / 255, \
		(float)((color & 0x00ff0000) >> 16) / 255, \
		(float)((color & 0x0000ff00) >> 8)  / 255, \
		(float)((color & 0x000000ff)) / 255        \
	)

#define SCALE_RGBA_ARRAY(r, g, b, a) (float)(r) / 255, (float)(g) / 255, (float)(b) / 255, (float)(a) / 255

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

