#include <string>
#include "imbase/launcher.hpp"

using namespace ImBase;

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

void Launcher::OpenUrl(const char* const url) {
#if defined(TARGET_WINDOWS)
	ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif defined(TARGET_APPLE)
	system(("open \"" + std::string(url) + "\"").c_str());
#else
	system(("setsid xdg-open \"" + std::string(url) + "\"").c_str());
#endif
}


