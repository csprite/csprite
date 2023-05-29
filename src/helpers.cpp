#include "helpers.hpp"

#include <cstdio>

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

void openUrl(std::string url) {
#if defined(TARGET_WINDOWS)
	ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#elif defined(TARGET_APPLE)
	system(("open \"" + url + "\"").c_str());
#else
	system(("xdg-open \"" + url + "\"").c_str());
#endif
}

