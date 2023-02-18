#include "imgui.h"
#include "../imgui_extension.h"
#include "preferences.h"
#include "../config.h"
#include "../main.h"

bool ShowPreferencesWindow = false;

void PreferencesUI_Open(void) {
	ShowPreferencesWindow = true;
}

void PreferencesUI_Close(void) {
	ShowPreferencesWindow = false;
}

void PreferencesUI_Draw(void) {
	Config_T* AppConfig = GetAppConfig();
	if (ShowPreferencesWindow) {
		if (ImGui::BeginPopupModal("Preferences###PreferencesWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("VSync (%s)", AppConfig->vsync ? "Enabled" : "Disabled");
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Vertical Sync - Synchronize App's FPS To Your Monitor's FPS");
			ImGui::SameLine();
			ImGui::Ext_ToggleButton("VSync_Toggle", &AppConfig->vsync);

			ImGui::InputInt("FPS", &AppConfig->FramesUpdateRate, 1, 5, AppConfig->vsync == true ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Frames Per Second");

			AppConfig->FramesUpdateRate = AppConfig->FramesUpdateRate < 5 ? 5 : AppConfig->FramesUpdateRate;
			if (ImGui::Button("Save")) {
				WriteConfig(AppConfig);
				SetFrameRate(AppConfig->FramesUpdateRate);
				PreferencesUI_Close();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("please restart the app after saving");
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				PreferencesUI_Close();
			}
			ImGui::EndPopup();
		} else {
			ImGui::OpenPopup("Preferences###PreferencesWindow");
		}
	}
}

