#include "image/ui.hpp"

bool ImageLayers_UI_Draw(Image& img, u32& activeLayer, bool& ShowLayerPropertiesWindow) {
	bool res = false;
	ImGui::SeparatorText("Layers");

	if (ImGui::Button("+")) {
		img.AddLayer();
		activeLayer = img.Layers.size() - 1;
		res = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("-") && img.Layers.size() > 0) {
		img.RemoveLayer(activeLayer);
		activeLayer = img.Layers.size() - 1;
		res = true;
	}

	ImGui::BeginChild("##LayersList", { 0, 0 }, true);

	for (size_t i = 0; i < img.Layers.size(); i++) {
		const Layer& layer = img.Layers[i];
		ImGui::PushID(i);
		if (ImGui::Selectable(layer.name.c_str(), i == activeLayer, ImGuiSelectableFlags_AllowDoubleClick)) {
			activeLayer = i;

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				ShowLayerPropertiesWindow = true;
			}
		}
		ImGui::PopID();
	}

	ImGui::EndChild();

	return res;
}
