#include "manager.hpp"
#include "fs/fs.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <cstdlib>
#include <fstream>
#include <array>

static std::vector<String> LanguageFiles;
static UiTextItemArr Language;

UiTextItemArr& LanguageManager::Get() {
	return Language;
}

void LanguageManager::UpdateEntries() {
	LanguageFiles.clear();
	Fs::ListDir(Fs::GetLanguagesDir().c_str(), [&](const char* entryName, bool isFile) -> bool {
		if (isFile) {
			LanguageFiles.push_back(entryName);
		}
		return true;
	});
}

void LanguageManager::ListAll(OnListCB cb) {
	for (auto const& filePath : LanguageFiles) {
		cb(filePath.c_str());
	}
}

const char* GetPropertyName(UI_TEXT i);
const char* GetEntryName(UI_TEXT i);

char* string_dup(const char* const str) {
	if (str) {
		u32 len = strlen(str);
		char* s = new char[len + 1]();
		memcpy(s, str, len);
		return s;
	}

	return nullptr;
}

bool LanguageManager::LoadFile(const String& filePath) {
	if (Fs::IsRegularFile(filePath) != 1) {
		return false;
	}

	std::ifstream f(filePath);
	json p = json::parse(f);

	for (u32 i = 0; i < UI_TEXT::COUNT; i++) {
		if (Language[i] != nullptr)
			delete[] Language[i];

		Language[i] = string_dup(p[GetPropertyName((UI_TEXT)i)][GetEntryName((UI_TEXT)i)].get<String>().c_str());
	}

	return true;
}

const char* GetEntryName(UI_TEXT i) {
	switch (i) {
		case UI_TEXT::MENU_FILE: return "File";
		case UI_TEXT::MENU_NEW: return "New";
		case UI_TEXT::MENU_OPEN: return "Open";

		case UI_TEXT::MENU_HELP: return "Help";
		case UI_TEXT::MENU_ABOUT: return "About";
		case UI_TEXT::MENU_GITHUB: return "GitHub";

		case UI_TEXT::POPUP_OPEN_FILE: return "OpenFile";

		case UI_TEXT::POPUP_NEW_DOCUMENT: return "NewDocument";
		case UI_TEXT::POPUP_NEW_DOCUMENT_WIDTH_INPUT: return "WidthInput";
		case UI_TEXT::POPUP_NEW_DOCUMENT_HEIGHT_INPUT: return "HeightInput";
		case UI_TEXT::POPUP_NEW_DOCUMENT_OK_BUTTON: return "OkButton";
		case UI_TEXT::POPUP_NEW_DOCUMENT_CANCEL_BUTTON: return "CancelButton";

		case UI_TEXT::POPUP_ABOUT_CSPRITE: return "AboutCsprite";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_HEADER: return "ContributorHeader";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_PARAGRAPH: return "ContributorParagraph";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_LINK: return "ContributorLink";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_OSPROJECTS_HEADER: return "OsProjectsHeader";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_OSPROJECTS_TEXT: return "OsProjectsText";

		default: return "";
	}
}

const char* GetPropertyName(UI_TEXT i) {
	switch (i) {
		case UI_TEXT::MENU_FILE: return "FileMenu";
		case UI_TEXT::MENU_NEW: return "FileMenu";
		case UI_TEXT::MENU_OPEN: return "FileMenu";

		case UI_TEXT::MENU_HELP: return "HelpMenu";
		case UI_TEXT::MENU_ABOUT: return "HelpMenu";
		case UI_TEXT::MENU_GITHUB: return "HelpMenu";

		case UI_TEXT::POPUP_OPEN_FILE: return "OpenFilePopup";

		case UI_TEXT::POPUP_NEW_DOCUMENT: return "NewDocumentPopup";
		case UI_TEXT::POPUP_NEW_DOCUMENT_WIDTH_INPUT: return "NewDocumentPopup";
		case UI_TEXT::POPUP_NEW_DOCUMENT_HEIGHT_INPUT: return "NewDocumentPopup";
		case UI_TEXT::POPUP_NEW_DOCUMENT_OK_BUTTON: return "NewDocumentPopup";
		case UI_TEXT::POPUP_NEW_DOCUMENT_CANCEL_BUTTON: return "NewDocumentPopup";

		case UI_TEXT::POPUP_ABOUT_CSPRITE: return "AboutCspritePopup";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_HEADER: return "AboutCspritePopup";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_PARAGRAPH: return "AboutCspritePopup";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_LINK: return "AboutCspritePopup";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_OSPROJECTS_HEADER: return "AboutCspritePopup";
		case UI_TEXT::POPUP_ABOUT_CSPRITE_OSPROJECTS_TEXT: return "AboutCspritePopup";

		default: return "";
	}
}

void LanguageManager::LoadDefault() {
	LanguageManager::LoadFile(Fs::GetLanguagesDir() + SYS_PATH_SEP + "english.json");
}

