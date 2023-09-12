#include <sys/stat.h>
#include "fs/fs.hpp"

#if defined(TARGET_LINUX)
	#include <unistd.h>
	#include <pwd.h>
#elif defined(TARGET_WINDOWS)
	#include <direct.h>
	#include "filebrowser/Dirent/dirent.h"
#endif

#ifndef TARGET_WINDOWS
	#include <dirent.h>
#endif

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

int Fs::MakeDir(const char* const path) {
#if defined(TARGET_WINDOWS)
	return _mkdir(path);
#else
	return mkdir(path, S_IRWXU);
#endif
}

int Fs::MakeDirRecursive(const String& _p) {
	String path = _p;

	if (path.back() == SYS_PATH_SEP_CHAR) path.pop_back();

	for (u32 i = 0; i < path.length(); ++i) {
		if (path[i] == SYS_PATH_SEP_CHAR) {
			if (Fs::MakeDir(path.substr(0, i + 1).c_str()) != 0 && errno != EEXIST) {
				return -1;
			}
		}
	}

	return Fs::MakeDir(path.c_str());
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

i32 Fs::ListDir(const char* dirPath, OnListDirCB cb) {
	DIR* dir = opendir(dirPath);
	if (dir == NULL)
		return -1;

	int i = 0;
	while (true) {
		errno = 0;
		struct dirent* dirent = readdir(dir);
		if (dirent == NULL || errno != 0) {
			closedir(dir);
			return -1;
		} else if (dirent->d_name[0] == '.') {
			continue; // skip "." & ".." entries
		} else if (cb(dirent->d_name, Fs::IsRegularFile(String(dirPath) + SYS_PATH_SEP + dirent->d_name)) == false) {
			break;
		}

		i++;
	}

	closedir(dir);
	return i;
}

