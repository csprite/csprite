#ifndef CSP_APP_PREFERENCES_HPP_INCLUDED_
#define CSP_APP_PREFERENCES_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

struct Preferences {
	u32 fps = 50;
	String langFileName = "english.ini";

	bool Load(const char* filePath);
	void Write(const char* filePath);
};

#endif // CSP_APP_PREFERENCES_HPP_INCLUDED_

