#include "system.h"

#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#if defined(__unix__) || defined(__linux__)
	#include <unistd.h>
	#include <pwd.h>
#elif defined(_WIN32)
	#include <windows.h>
	#include <shellapi.h>
#endif

// On Windows Mkdir Only Takes Path So We Don't Pass Path
#if defined(_WIN32) || defined(WIN32)
	#define _mkdir_custom(path, perms) mkdir(path)
#else
	#define _mkdir_custom(path, perms) mkdir(path, perms)
#endif

// Opens The Given URL in default browser where URL's max size SYS_URL_MAX, If no implementation found logs a msg in console
void Sys_OpenURL(const char* URL) {
	#if defined(_WIN32)
		ShellExecute(0, 0, URL, 0, 0, SW_SHOWNORMAL);
	#elif defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
		#ifdef __APPLE__
			#define OPEN_CMD "open"
		#else
			#define OPEN_CMD "xdg-open"
		#endif
		char command[SYS_URL_MAX + 15] = "";
		snprintf(command, SYS_URL_MAX + 15, OPEN_CMD " \"%s\"", URL);
		system(command);
	#else
		printf("Cannot open url: %s, No function implementation found!", URL);
	#endif
}

// Returns File Size Using A FILE*
size_t Sys_GetFileSize(FILE* fp) {
	if (fp == NULL) return 0;
	size_t CurrPos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, CurrPos, SEEK_SET); // Seek To Previous Position
	return size;
}

// Checks if a file path is a normal readable file or not, returns 1 if regular, 0 if not, -1 on error (with errno set)
int Sys_IsRegularFile(const char* filePath) {
	struct stat st;

	if (stat(filePath, &st) < 0)
		return -1;

	return S_ISREG(st.st_mode);
}

// Checks if a directory path is a normal openable path or not, returns 1 if regular, 0 if not, -1 on error (with errno set)
int Sys_IsRegularDir(const char* dirPath) {
	struct stat st;

	if (stat(dirPath, &st) < 0)
		return -1;

	return S_ISDIR(st.st_mode);
}

char* Sys_GetConfigDir(void) {
	static char fullPath[SYS_PATHNAME_MAX] = "";

#if defined(__unix__) || defined(__linux__)
	const char* configHome = NULL;
	configHome = getenv("XDG_CONFIG_HOME");
	if (!configHome) {
		configHome = getenv("HOME");
		if (!configHome) {
			configHome = getpwuid(getuid())->pw_dir;
			if (!configHome) return NULL;
		}
		snprintf(fullPath, SYS_PATHNAME_MAX, "%s/.config", configHome);
	} else {
		snprintf(fullPath, SYS_PATHNAME_MAX, "%s", configHome);
	}
#elif defined(_WIN32) || defined(WIN32)
	const char* appdata = getenv("APPDATA");
	strncpy(fullPath, appdata, SYS_PATHNAME_MAX - 1);
#elif defined(__APPLE__) || defined(__MACH__)
	const char* home = getenv("HOME");
	snprintf(fullPath, SYS_PATHNAME_MAX - 1, "%s/Library/Application Support", home);
#endif

	return fullPath;
}

// get the filename where csprite's log will be writen to.
char* Sys_GetLogFileName(void) {
	static char path[SYS_PATHNAME_MAX] = "";
	/* only executed if variable path is empty.
	   since it's a static variable there's no need to append,
	   the same information again as it will persist till the life-time of the program */
	if (path[0] == 0) {
		char* configDir = Sys_GetConfigDir();
		snprintf(path, SYS_PATHNAME_MAX, "%s/csprite/csprite.log", configDir);
	}
	return path;
}

void Sys_MakeDirRecursive(const char* dir) {
	char tmp[SYS_PATHNAME_MAX];
	char* p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);

	if (tmp[len - 1] == SYS_PATH_SEP) tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++) {
		if (*p == SYS_PATH_SEP) {
			*p = 0;
			_mkdir_custom(tmp, S_IRWXU);
			*p = SYS_PATH_SEP;
		}
	}

	_mkdir_custom(tmp, S_IRWXU);
}

char* Sys_GetBasename(const char* path) {
	if (path == NULL) return NULL;

	int pathLen = strlen(path);
	int lastSepIndex = 0;

	for (int i = 0; i < pathLen; ++i) {
		if (path[i] == '\\' || path[i] == '/') lastSepIndex = i;
	}
	lastSepIndex++;

	int fnameLen = pathLen - lastSepIndex;
	char* fileName = malloc((fnameLen + 1) * sizeof(char));
	memset(fileName, '\0', fnameLen + 1);
	strncpy(fileName, path + lastSepIndex, pathLen - lastSepIndex);

	return fileName;
}

int Sys_ListDirContents(const char* dirpath, int (*callback)(const char *dirpath, const char *name, void* data), void* data) {
	int i = 0;
	DIR *dir;
	struct dirent *dirent;
	dir = opendir(dirpath);
	if (!dir) return -1;
	while ((dirent = readdir(dir))) {
		if (dirent->d_name[0] == '.') continue;
		if (callback != NULL)
			if (callback(dirpath, dirent->d_name, data) != 0)
				break;

		i++;
	}
	closedir(dir);
	return i;
}
