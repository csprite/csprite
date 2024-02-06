#include <fstream>
#include <string> // std::stoul

#include "app/assets/assets.h"
#include "app/i18n/strings.hpp"
#include "app/fswrapper.hpp"
#include "fs/fs.hpp"
#include "log/log.h"

#include "SimpleIni.h"

namespace Fs = FileSystem;

static std::vector<String> LanguageFiles;
static UISTR_Arr Language;
static ImVector<ImWchar> Range;

const UISTR_Arr& UIString::Get() {
	return Language;
}

const ImWchar* UIString::GetRanges() {
	return Range.Data;
}

void UIString::UpdateEntries() {
	LanguageFiles.clear();
	Fs::ListDir(Fs::GetLanguagesDir(), [&](const String& entryName, bool isFile) -> bool {
		// printf("%s\n", entryName.c_str());
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

void _ParseRange(CSimpleIniA& ini) {
	Range.clear();
	//~ if (p.contains("UnicodeRange")) {
		//~ Vector<String> RangeStr = p["UnicodeRange"].get<Vector<String>>();
		//~ for (const String& s : RangeStr) {
			//~ Range.push_back(std::stoul(s, nullptr, 16));
		//~ }
		//~ Range.push_back(0);
	//~ }
	Range.push_back(0);
}

bool UIString::LoadFile(const String& fileName) {
	String filePath = Fs::GetLanguagesDir() + PATH_SEP + fileName;

	if (Fs::IsRegularFile(filePath) != 1) {
		return false;
	}

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(filePath.c_str());

	UIString::LoadDefault();

	for (u32 i = 0; i < UISTR::COUNT; i++) {
		const char* str = ini.GetValue(GetPropertyName((UISTR)i), GetEntryName((UISTR)i), NULL);

		// This Logic Reduces Re-Duplication of String
		if (str == NULL) {
			str = Language[i];
		} else {
			str = string_dup(str);
			if (Language[i] != nullptr) {
				delete[] Language[i];
			}
		}

		Language[i] = str;
	}
	_ParseRange(ini);

	return true;
}

void UIString::LoadDefault() {
	i32 sz = 0;
	const char* data = (const char*)assets_get("assets/languages/english.ini", &sz);

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadData(data, sz);

	for (u32 i = 0; i < UISTR::COUNT; i++) {
		if (Language[i] != nullptr)
			delete[] Language[i];

		Language[i] = string_dup(ini.GetValue(GetPropertyName((UISTR)i), GetEntryName((UISTR)i), "ERROR_404"));
	}

	_ParseRange(ini);
}

const char* GetEntryName(UISTR i) {
	switch (i) {
		case UISTR::Menu_File: return "file";
		case UISTR::MenuItem_New: return "new";
		case UISTR::MenuItem_Open: return "open";

		case UISTR::Menu_Help: return "help";
		case UISTR::MenuItem_About: return "about";
		case UISTR::MenuItem_GitHub: return "github";

		case UISTR::Popup_OpenFile: return "open_file";

		case UISTR::Popup_NewDocument: return "title";
		case UISTR::Popup_NewDocument_WidthInput: return "width_input";
		case UISTR::Popup_NewDocument_HeightInput: return "height_input";
		case UISTR::Popup_NewDocument_OkButton: return "ok_button";
		case UISTR::Popup_NewDocument_CancelButton: return "cancel_button";

		case UISTR::Popup_AboutCsprite: return "title";
		case UISTR::Popup_AboutCsprite_Contrib_Header: return "contributors_header";
		case UISTR::Popup_AboutCsprite_Contrib_Paragraph: return "contributors_paragraph";
		case UISTR::Popup_AboutCsprite_Contrib_Link: return "contributors_link";
		case UISTR::Popup_AboutCsprite_OpenSrcProjects_Header: return "os_projects_header";
		case UISTR::Popup_AboutCsprite_OpenSrcProjects_Text: return "os_projects_text";
		case UISTR::Popup_AboutCsprite_CloseButton: return "close_button";

		default: return "";
	}
}

const char* GetPropertyName(UISTR i) {
	switch (i) {
		case UISTR::Menu_File:
		case UISTR::MenuItem_New:
		case UISTR::MenuItem_Open: {
			return "file_menu";
		}

		case UISTR::Menu_Help:
		case UISTR::MenuItem_About:
		case UISTR::MenuItem_GitHub: {
			return "help_menu";
		}

		case UISTR::Popup_OpenFile: {
			return "open_file_popup";
		}

		case UISTR::Popup_NewDocument:
		case UISTR::Popup_NewDocument_WidthInput:
		case UISTR::Popup_NewDocument_HeightInput:
		case UISTR::Popup_NewDocument_OkButton:
		case UISTR::Popup_NewDocument_CancelButton: {
			return "new_document_popup";
		}

		case UISTR::Popup_AboutCsprite:
		case UISTR::Popup_AboutCsprite_Contrib_Header:
		case UISTR::Popup_AboutCsprite_Contrib_Paragraph:
		case UISTR::Popup_AboutCsprite_Contrib_Link:
		case UISTR::Popup_AboutCsprite_OpenSrcProjects_Header:
		case UISTR::Popup_AboutCsprite_OpenSrcProjects_Text:
		case UISTR::Popup_AboutCsprite_CloseButton: {
			return "about_csprite_popup";
		}

		default: return "";
	}
}
