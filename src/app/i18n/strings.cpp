#include <fstream>

#include "app/i18n/strings.hpp"
#include "fs/fs.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

static std::vector<String> LanguageFiles;
static UISTR_Arr Language;

UISTR_Arr UIString::Get() {
	return Language;
}

void UIString::UpdateEntries() {
	LanguageFiles.clear();
	Fs::ListDir(Fs::GetLanguagesDir().c_str(), [&](const char* entryName, bool isFile) -> bool {
		if (isFile) {
			LanguageFiles.push_back(entryName);
		}
		return true;
	});
}

void UIString::ListAll(OnListCB cb) {
	for (auto const& filePath : LanguageFiles) {
		cb(filePath.c_str());
	}
}

const char* GetPropertyName(UISTR i);
const char* GetEntryName(UISTR i);

char* string_dup(const char* const str) {
	if (str) {
		u32 len = strlen(str);
		char* s = new char[len + 1]();
		memcpy(s, str, len);
		return s;
	}

	return nullptr;
}

bool UIString::LoadFile(const String& filePath) {
	if (Fs::IsRegularFile(filePath) != 1) {
		return false;
	}

	std::ifstream f(filePath);
	json p = json::parse(f);

	for (u32 i = 0; i < UISTR::COUNT; i++) {
		if (Language[i] != nullptr)
			delete[] Language[i];

		Language[i] = string_dup(p[GetPropertyName((UISTR)i)][GetEntryName((UISTR)i)].get<String>().c_str());
	}

	return true;
}

const char* GetEntryName(UISTR i) {
	switch (i) {
		case UISTR::MENU_FILE: return "File";
		case UISTR::MENU_NEW: return "New";
		case UISTR::MENU_OPEN: return "Open";

		case UISTR::MENU_HELP: return "Help";
		case UISTR::MENU_ABOUT: return "About";
		case UISTR::MENU_GITHUB: return "GitHub";

		case UISTR::POPUP_OPEN_FILE: return "OpenFile";

		case UISTR::POPUP_NEW_DOCUMENT: return "NewDocument";
		case UISTR::POPUP_NEW_DOCUMENT_WIDTH_INPUT: return "WidthInput";
		case UISTR::POPUP_NEW_DOCUMENT_HEIGHT_INPUT: return "HeightInput";
		case UISTR::POPUP_NEW_DOCUMENT_OK_BUTTON: return "OkButton";
		case UISTR::POPUP_NEW_DOCUMENT_CANCEL_BUTTON: return "CancelButton";

		case UISTR::POPUP_ABOUT_CSPRITE: return "AboutCsprite";
		case UISTR::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_HEADER: return "ContributorHeader";
		case UISTR::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_PARAGRAPH: return "ContributorParagraph";
		case UISTR::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_LINK: return "ContributorLink";
		case UISTR::POPUP_ABOUT_CSPRITE_OSPROJECTS_HEADER: return "OsProjectsHeader";
		case UISTR::POPUP_ABOUT_CSPRITE_OSPROJECTS_TEXT: return "OsProjectsText";

		default: return "";
	}
}

const char* GetPropertyName(UISTR i) {
	switch (i) {
		case UISTR::MENU_FILE: return "FileMenu";
		case UISTR::MENU_NEW: return "FileMenu";
		case UISTR::MENU_OPEN: return "FileMenu";

		case UISTR::MENU_HELP: return "HelpMenu";
		case UISTR::MENU_ABOUT: return "HelpMenu";
		case UISTR::MENU_GITHUB: return "HelpMenu";

		case UISTR::POPUP_OPEN_FILE: return "OpenFilePopup";

		case UISTR::POPUP_NEW_DOCUMENT: return "NewDocumentPopup";
		case UISTR::POPUP_NEW_DOCUMENT_WIDTH_INPUT: return "NewDocumentPopup";
		case UISTR::POPUP_NEW_DOCUMENT_HEIGHT_INPUT: return "NewDocumentPopup";
		case UISTR::POPUP_NEW_DOCUMENT_OK_BUTTON: return "NewDocumentPopup";
		case UISTR::POPUP_NEW_DOCUMENT_CANCEL_BUTTON: return "NewDocumentPopup";

		case UISTR::POPUP_ABOUT_CSPRITE: return "AboutCspritePopup";
		case UISTR::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_HEADER: return "AboutCspritePopup";
		case UISTR::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_PARAGRAPH: return "AboutCspritePopup";
		case UISTR::POPUP_ABOUT_CSPRITE_CONTRIBUTOR_LINK: return "AboutCspritePopup";
		case UISTR::POPUP_ABOUT_CSPRITE_OSPROJECTS_HEADER: return "AboutCspritePopup";
		case UISTR::POPUP_ABOUT_CSPRITE_OSPROJECTS_TEXT: return "AboutCspritePopup";

		default: return "";
	}
}

void UIString::LoadDefault() {
	UIString::LoadFile(Fs::GetLanguagesDir() + SYS_PATH_SEP + "english.json");
}

