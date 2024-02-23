#include "palette/ui.hpp"
#include "imgui/imgui.h"

void Palette_UI_Draw(Palette& p, u32& index, Pixel& color) {
	ImGui::SeparatorText("Colors");

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });
	for (auto i = 0UL; i < p.Colors.size(); i++) {
		ImGui::PushID(&p[i]);

		if (ImGui::ColorButton(
			index == i ? "Selected Color" : "Color", p[i],
			ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder,
			{ ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f })
		) {
			index = i;
			color = p[i];
		}

		bool isColorSelected = index == i && color == p[i];
		if (isColorSelected || ImGui::IsItemHovered()) {
			ImVec2 rSz = ImGui::GetItemRectSize();
			ImVec2 rMin = ImGui::GetItemRectMin();
			ImVec2 rMax = ImGui::GetItemRectMax();

			u8 r = p.Colors[i].r;
			u8 g = p.Colors[i].g;
			u8 b = p.Colors[i].b;

			r = MIN_MAX((r > 127 ? r - 125 : r + 125), 0, 255);
			g = MIN_MAX((g > 127 ? g - 125 : g + 125), 0, 255);
			b = MIN_MAX((b > 127 ? b - 125 : b + 125), 0, 255);

			/* This Value Will Be Subtracted From Triangle's Positions
			   Because Of Some Extra "Marginal" Space The Button Takes */
			#define NEGATIVE_OFFSET 0.5f
			if (isColorSelected) {
				ImGui::GetWindowDrawList()->AddTriangleFilled(
					ImVec2(
						rMax.x - NEGATIVE_OFFSET,
						rMin.y + (rSz.y / 2.5f) - NEGATIVE_OFFSET
					),
					ImVec2(
						rMin.x + (rSz.x / 2.5f) - NEGATIVE_OFFSET,
						rMax.y - NEGATIVE_OFFSET
					),
					ImVec2(
						rMax.x - NEGATIVE_OFFSET,
						rMax.y - NEGATIVE_OFFSET
					),
					IM_COL32(r, g, b, 200)
				);
			}
			ImGui::GetWindowDrawList()->AddRect(
				rMin, rMax, IM_COL32(r, g, b, 200),
				0, 0, isColorSelected ? 1.0f : 2.0f
			);
			#undef NEGATIVE_OFFSET
		}

		// Expected position if next button was on same line
		float nextBtnSizeX = ImGui::GetItemRectMax().x + ImGui::GetItemRectSize().x + ImGui::GetStyle().ItemSpacing.x;
		if (
			i < p.Colors.size() - 1 &&
			nextBtnSizeX < (ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x)
		) ImGui::SameLine();

		ImGui::PopID();
	};
	ImGui::PopStyleVar(1); // ImGuiStyleVar_ItemSpacing

	float ColorPicker[4] = {
		color.r / 255.0f,
		color.g / 255.0f,
		color.b / 255.0f,
		color.a / 255.0f
	};
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::ColorPicker4("##ColorPicker", (float*)&ColorPicker, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)) {
		color.r = ColorPicker[0] * 255;
		color.g = ColorPicker[1] * 255;
		color.b = ColorPicker[2] * 255;
		color.a = ColorPicker[3] * 255;
	}
}
