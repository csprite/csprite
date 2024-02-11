#include <cstdlib> // getenv
#include "fswrapper.hpp"

#if defined(TARGET_LINUX)
	#include <unistd.h>
	#include <pwd.h>
#endif

String FileSystem::GetConfigDir() {
	String fullPath = "";

#if defined(TARGET_LINUX)
	const char* configHome = NULL;
	configHome = getenv("XDG_CONFIG_HOME");
	if (!configHome) {
		configHome = getenv("HOME");
		if (!configHome) {
			configHome = getpwuid(getuid())->pw_dir;
			if (!configHome) return NULL;
		}
		fullPath = String(configHome) + PATH_SEP ".config" PATH_SEP "csprite";
	} else {
		fullPath = String(configHome) + PATH_SEP "csprite";
	}
#elif defined(TARGET_WINDOWS)
	fullPath = "." PATH_SEP;
#elif defined(TARGET_APPLE)
	const char* home = getenv("HOME");
	fullPath = String(home) + PATH_SEP "Library/Application Support";
#else
	#error "No Implementation of Sys::GetConfigDir, please report this issue!"
#endif

	return fullPath;
}

