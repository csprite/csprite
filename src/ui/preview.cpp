#include <cstdint>
#include "imgui.h"
#include "preview.h"
#include "../renderer/canvas.h"
#include "../main.h"

bool isOpen = true;
int32_t PreviewZoom = 2;
ImVec2  PreviewImageSize;
bool ResetPreviewWindowPos = true;
bool ReCalculateZoomSize = true;
int32_t w = 0, h = 0; // canvas size

bool PreviewUI_IsOpen(void) { return isOpen; }
void PreviewUI_Open(void) { isOpen = true; }
void PreviewUI_Close(void) { isOpen = false; }

void PreviewUI_ResetPosition(void) { ResetPreviewWindowPos = true; }

void PreviewUI_Draw(void) {
		{
			// simple condition that set ResetPreviewWindowPos to true, so that Preview window position can be updated after the window width is calculated
			static int FramesPassed = 0;
			static bool LockCond = false;
			if (!LockCond) {
				if (FramesPassed != 3) {
					FramesPassed++;
				} else {
					LockCond = true;
					ResetPreviewWindowPos = true;
				}
			}
		}

		if (isOpen && ImGui::Begin("Preview", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::BeginMenu("Zoom")) {
					if (ImGui::MenuItem("Increase")) { PreviewZoom++; ReCalculateZoomSize = true; }
					if (ImGui::MenuItem("Decrease")) { if (PreviewZoom > 1) { PreviewZoom--; ReCalculateZoomSize = true; } }
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Reset Position")) {
					ResetPreviewWindowPos = true;
				}
				ImGui::EndPopup();
			}

			if (ReCalculateZoomSize) {
				GetCanvasDims(&w, &h);
				PreviewImageSize.x = w * PreviewZoom;
				PreviewImageSize.y = h * PreviewZoom;
				ReCalculateZoomSize = false;
			}
			if (ResetPreviewWindowPos == true) {
				ImGui::SetWindowPos({ ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x - 20, ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y - 20, }); // Move Window To Bottom Right With 20 pixel padding
				ResetPreviewWindowPos = false;
			}

			ImGui::Image(reinterpret_cast<ImTextureID>(Canvas_GetTex()), PreviewImageSize);
			ImGui::End();
		}
}


