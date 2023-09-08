#include <sys/stat.h>

#if defined(TARGET_LINUX)
	#include <unistd.h>
	#include <pwd.h>
	#include <dirent.h>
#elif defined(TARGET_WINDOWS)
	#include "filebrowser/Dirent/dirent.h"
#endif

#include "fs/fs.hpp"

String Fs::GetConfigDir() {
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
		fullPath = String(configHome) + SYS_PATH_SEP ".config" SYS_PATH_SEP "csprite";
	} else {
		fullPath = String(configHome) + SYS_PATH_SEP "csprite";
	}
#elif defined(TARGET_WINDOWS)
	const char* appdata = getenv("APPDATA");
	fullPath = String(appdata) + SYS_PATH_SEP "csprite";
#elif defined(TARGET_APPLE)
	const char* home = getenv("HOME");
	fullPath = String(home) + SYS_PATH_SEP "Library/Application Support";
#else
	#error "No Implementation of Sys::GetConfigDir, please report this issue!"
#endif

	return fullPath;
}

String Fs::GetParentDir(const String &path) {
	// using - 2 since it will filter out any trailing path separator
	for (auto i = path.length() - 2; i >= 0; --i) {
		if (path[i] == SYS_PATH_SEP[0]) {
			return path.substr(0, i);
		}
	}

	return "";
}

String Fs::GetBaseName(const String &path) {
	// using - 2 since it will filter out any trailing path separator
	for (auto i = path.length() - 2; i >= 0; --i) {
		if (path[i] == SYS_PATH_SEP[0]) {
			return path.substr(i + 1, path.length() - 1);
		}
	}

	return "";
}

i32 Fs::GetFileSize(const String &filePath) {
	FILE* f = fopen(filePath.c_str(), "r");
	if (f == NULL) return -1;
	if (fseek(f, 0, SEEK_END) < 0) {
		fclose(f);
		return -1;
	}

	i32 size = ftell(f); // on error ftell returns -1 which can be directly returned.
	fclose(f);

	return size;
}

// returns 1 if regular, 0 if not, -1 on error, with errno set
i32 Fs::IsRegularFile(const String& filePath) {
	struct stat st;

	if (stat(filePath.c_str(), &st) < 0)
		return -1;

	return S_ISREG(st.st_mode);
}

// returns 1 if regular, 0 if not, -1 on error, with errno set
i32 Fs::IsRegularDir(const String& dirPath) {
	struct stat st;

	if (stat(dirPath.c_str(), &st) < 0)
		return -1;

	return S_ISDIR(st.st_mode);
}
