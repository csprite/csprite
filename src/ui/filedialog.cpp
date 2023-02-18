#include "imgui.h"
#include "filedialog.h"
#include "ImGuiFileBrowser.h"

#include "../main.h"
#include "../ifileio/ifileio.h"
#include "../renderer/canvas.h"
#include "../utils.h"

imgui_addons::ImGuiFileBrowser* ImFileDialog = NULL;
bool _ShowOpen = false;
bool _ShowSave = false;

void FileDialogUI_Init() {
	if (ImFileDialog == NULL) {
		ImFileDialog = new imgui_addons::ImGuiFileBrowser;
	}
}

void FileDialogUI_DeInit() {
	delete ImFileDialog;
	ImFileDialog = NULL;
}

void FileDialogUI_Open_OpenFile() {
	_ShowOpen = true;
}
void FileDialogUI_Open_SaveFile() {
	_ShowSave = true;
	ImGui::OpenPopup("Save file as##Csprite_SaveAsFileDlg");
}

void FileDialogUI_Draw() {
	if (_ShowOpen) ImGui::OpenPopup("Select a file##Csprite_OpenNewFileDlg");
	if (_ShowSave) ImGui::OpenPopup("Save file as##Csprite_SaveAsFileDlg");

	if (ImFileDialog->showFileDialog("Select a file##Csprite_OpenNewFileDlg", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), ".csprite,.png,.jpg,.jpeg,.bmp")) {
		OpenNewFile(ImFileDialog->selected_path.c_str());
		_ShowOpen = false;
	}

	if (ImFileDialog->showFileDialog("Save file as##Csprite_SaveAsFileDlg", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(0, 0), ".csprite,.png,.jpg,.jpeg,.bmp")) {
		/*  simple logic which appends the appropriate extension if none provided.
			like "test1" with ".png" filter selected is converted to "test1.png" */
		char* _fPath = (char*)ImFileDialog->selected_path.c_str();
		if (!HAS_SUFFIX_CI(_fPath, ImFileDialog->ext.c_str(), ImFileDialog->ext.length())) {
			int newStrLen = ImFileDialog->selected_path.length() + ImFileDialog->ext.length() + 1;
			_fPath = (char*)malloc(newStrLen);
			snprintf(_fPath, newStrLen, "%s%s", ImFileDialog->selected_path.c_str(), ImFileDialog->ext.c_str());
		}

		int32_t w = 0, h = 0;
		CanvasLayerArr_T* layers = GetCanvasLayers();
		GetCanvasDims(&w, &h);
		if (w > 0 && h > 0 && layers != NULL && _fPath != NULL) {
			if (ifio_write(_fPath, w, h, layers) == 0) {
				SetFilePath(_fPath);
			}
		}
		_ShowSave = false;
	}
}

