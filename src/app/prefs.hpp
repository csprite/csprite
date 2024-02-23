#ifndef CSP_APP_PREFERENCES_HPP_INCLUDED_
#define CSP_APP_PREFERENCES_HPP_INCLUDED_ 1
#pragma once

#include "types.h"

struct Preferences {
	u32 fps = 50;
	u32 fontSize = 18;
	String langFileName = "";

	bool Load(const char* filePath);
	void Write(const char* filePath);
};

// Returns True to request closure of the popup/window
bool Prefs_UI_Draw(Preferences& prefs);

#endif // CSP_APP_PREFERENCES_HPP_INCLUDED_

