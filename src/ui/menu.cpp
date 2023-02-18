#include "imgui.h"
#include "menu.h"
#include "new_canvas.h"
#include "filedialog.h"
#include "preferences.h"
#include "preview.h"

#include "../main.h"
#include "../system.h"
#include "../ifileio/ifileio.h"
#include "../renderer/canvas.h"

void Menu_Draw() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New")) {
				NewCanvasUI_Open();
			}
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				FileDialogUI_Open_OpenFile();
			}
			if (ImGui::BeginMenu("Save")) {
				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					int32_t w = 0, h = 0;
					CanvasLayerArr_T* layers = NULL;
					const char* filePath = GetFilePath();
					GetCanvasDims(&w, &h);
					layers = GetCanvasLayers();
					if (w > 0 && h > 0 && layers != NULL && filePath != NULL) {
						ifio_write(filePath, w, h, layers);
					}
				}
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
					FileDialogUI_Open_SaveFile();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, CanUndo())) {
				Canvas_Undo();
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, CanRedo())) {
				Canvas_Redo();
			}
			if (ImGui::MenuItem("Preferences")) {
				PreferencesUI_Open();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Preview")) {
				if (PreviewUI_IsOpen()) PreviewUI_Close();
				else PreviewUI_Open();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("Wiki")) {
				Sys_OpenURL("https://csprite.github.io/wiki/");
			}
			if (ImGui::MenuItem("About")) {
				Sys_OpenURL("https://github.com/pegvin/csprite/wiki/About-CSprite");
			}
			if (ImGui::MenuItem("GitHub")) {
				Sys_OpenURL("https://github.com/pegvin/csprite");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

