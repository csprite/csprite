#include <cstdint>
#include "imgui.h"
#include "new_canvas.h"
#include "../main.h"

bool ShowNewCanvasWindow = false;
int32_t NewDims[2] = { 64, 64 };

void NewCanvasUI_Open() {
	ShowNewCanvasWindow = true;
}

void NewCanvasUI_Close() {
	NewDims[0] = 64;
	NewDims[1] = 64;
	ShowNewCanvasWindow = false;
}

bool NewCanvasUI_IsOpen() {
	return ShowNewCanvasWindow;
}

void NewCanvasUI_Draw() {
	if (ShowNewCanvasWindow) {
		if (ImGui::BeginPopupModal("Create New###NewCanvasWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputInt("Width", &NewDims[0], 1, 5);
			ImGui::InputInt("Height", &NewDims[1], 1, 5);

			if (ImGui::Button("Create")) {
				if (NewDims[0] > 0 && NewDims[1] > 0) {
					SetCanvasDims(NewDims[0], NewDims[1]);
				}
				ShowNewCanvasWindow = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				NewCanvasUI_Close();
			}
			ImGui::EndPopup();
		} else {
			ImGui::OpenPopup("Create New###NewCanvasWindow");
		}
	}
}


