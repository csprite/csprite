#include "SimpleIni.h"
#include "app/prefs.hpp"

bool Preferences::Load(const char* filePath) {
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(filePath) < 0) {
		return false;
	}

	fps = ini.GetLongValue("program", "fps", Preferences::fps);
	fps = MIN_MAX(fps, 5, 999);
	fontSize = ini.GetLongValue("program", "font_size", Preferences::fontSize);
	fontSize = MIN_MAX(fontSize, 10, 999);
	langFileName = ini.GetValue("program", "language_file", Preferences::langFileName.c_str());

	return true;
}

void Preferences::Write(const char* filePath) {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetLongValue("program", "fps", fps);
	ini.SetLongValue("program", "font_size", fontSize);
	ini.SetValue("program", "language_file", langFileName.c_str());
	ini.SaveFile(filePath);
}

#include "imgui/imgui.h"
#include "app/app.hh"
#include "app/i18n/strings.hpp"

bool Prefs_UI_Draw(Preferences& prefs) {
	static Preferences tempPrefs = prefs;
	static int32_t CurrentSelection = 0;

	ImGui::BeginTable("##PreferencesTable", 2, ImGuiTableFlags_BordersInnerV);
	ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, 115.0f, 0);
	ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);

	// set the row height to maximum available content height in the window
	auto vMin_y = ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y;
	auto vMax_y = ImGui::GetWindowContentRegionMax().y + ImGui::GetWindowPos().y;
	ImGui::TableNextRow(ImGuiTableRowFlags_None, (vMax_y - vMin_y) - ImGui::GetFrameHeightWithSpacing());

	ImGui::TableNextColumn();

	if (ImGui::Selectable("General", CurrentSelection == 0, ImGuiSelectableFlags_DontClosePopups)) {
		CurrentSelection = 0;
	} else if (ImGui::Selectable("Languages", CurrentSelection == 1, ImGuiSelectableFlags_DontClosePopups)) {
		CurrentSelection = 1;
	}

	ImGui::TableNextColumn();

	switch (CurrentSelection) {
		case 0: {
			u32 p_step = 1;
			u32 p_stepFast = 5;
			ImGui::InputScalar("Max FPS", ImGuiDataType_U32, &tempPrefs.fps, &p_step, &p_stepFast);
			tempPrefs.fps = tempPrefs.fps < 5 ? 5 : tempPrefs.fps;

			ImGui::InputScalar("Font Size", ImGuiDataType_U32, &tempPrefs.fontSize, &p_step, &p_stepFast);
			tempPrefs.fontSize = tempPrefs.fontSize < 10 ? 10 : tempPrefs.fontSize;
			break;
		}
		case 1: {
			if (ImGui::BeginCombo("##Language", tempPrefs.langFileName.c_str())) {
				UIString::ListAll([&](const char* fileName) {
					if (ImGui::Selectable(fileName)) {
						tempPrefs.langFileName = fileName;
					}
				});
				if (ImGui::Selectable("default")) {
					tempPrefs.langFileName.clear();
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine(0, 3);
			if (ImGui::Button("Refresh")) {
				UIString::UpdateEntries();
			}
			break;
		}
		default: {
			ImGui::TextWrapped("Not-Reachable Section: %d, Please Report This To The Developer", CurrentSelection);
			break;
		}
	}

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	bool res = false;

	if (ImGui::Button("Save")) {
		prefs = tempPrefs;
		prefs.Write(App_GetConfigFile().c_str());
		res = true;
		ImGui::CloseCurrentPopup();
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip("Restart app to apply these changes");
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		tempPrefs = prefs;
		res = true;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndTable();

	return res;
}
