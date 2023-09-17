#ifndef CSP_APP_LANGUAGE_MANAGER_HPP_INCLUDED_
#define CSP_APP_LANGUAGE_MANAGER_HPP_INCLUDED_ 1
#pragma once

#include <array>
#include <functional>
#include "types.hpp"
#include "imgui/imgui.h"

enum UISTR : u16 {
	Menu_File = 0,
		MenuItem_New,
		MenuItem_Open,

	Menu_Help,
		MenuItem_About,
		MenuItem_GitHub,

	Popup_OpenFile,
	Popup_NewDocument,
		Popup_NewDocument_WidthInput,
		Popup_NewDocument_HeightInput,
		Popup_NewDocument_OkButton,
		Popup_NewDocument_CancelButton,

	Popup_AboutCsprite,
		Popup_AboutCsprite_Contrib_Header,
		Popup_AboutCsprite_Contrib_Paragraph,
		Popup_AboutCsprite_Contrib_Link,

		Popup_AboutCsprite_OpenSrcProjects_Header,
		Popup_AboutCsprite_OpenSrcProjects_Text,

	COUNT
};

using UISTR_Arr = std::array<const char*, UISTR::COUNT>;

namespace UIString {
	typedef std::function<void(const char* name)> OnListCB;
	void ListAll(OnListCB cb);
	void UpdateEntries();
	bool LoadFile(const String& filePath);
	void LoadDefault();

	const UISTR_Arr& Get();
	const ImWchar* GetRanges();
}

#endif // CSP_APP_LANGUAGE_MANAGER_HPP_INCLUDED_
