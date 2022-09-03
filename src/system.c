#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "log/log.h"
#include "system.h"
#include "cconfig.h"

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#if defined(__unix__) && !defined(ANDROID)
	#include <pwd.h>
#endif

#include <stdio.h>

/*
	Returns File Size Using A FILE*
*/
long int fsize(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	long int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

/*
	Lists Contents Of A Directory
*/
int SysListDir(const char *dirpath, int (*callback)(const char *dirpath, const char *name, void* data), void* data) {
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

// On Windows Mkdir Only Takes Path So We Don't Pass Path
#if defined(_WIN32) || defined(WIN32)
	#define _mkdir_custom(path, perms) mkdir(path)
#else
	#define _mkdir_custom(path, perms) mkdir(path, perms)
#endif

/*
	Recursively Creates Directories
*/
int SysMakeDir(const char *dir) {
	char tmp[PATH_MAX];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);

	if (tmp[len - 1] == SYS_PATH_SEP)
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++)
		if (*p == SYS_PATH_SEP) {
			*p = 0;
			_mkdir_custom(tmp, S_IRWXU);
			*p = SYS_PATH_SEP;
		}
	_mkdir_custom(tmp, S_IRWXU);
	return 0;
}

/*
	Returns Palette Directory: $CSPRITE_CONFIG/palettes
*/
char* SysGetPaletteDir() {
	char* configdir = CCGetConfigDir();
	static char configPath[CC_PATH_SIZE_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_error("cannot get the config directory!");
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "palettes");
			SysMakeDir(configPath);
		} else {
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "%s%ccsprite%cpalettes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			SysMakeDir(configPath);
		}
	}

	return configPath;
}

/*
	Returns Theme Directory: $CSPRITE_CONFIG/themes
*/
char* SysGetThemesDir() {
	char* configdir = CCGetConfigDir();
	static char configPath[CC_PATH_SIZE_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_error("cannot get the themes directory!");
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "themes");
			SysMakeDir(configPath);
		} else {
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "%s%ccsprite%cthemes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			SysMakeDir(configPath);
		}
	}

	return configPath;
}

char* SysFnameFromPath(const char* path) {
	if (path == NULL) return NULL;

	int pathLen = strlen(path);
	int lastSepIndex = 0;

	for (int i = 0; i < pathLen; ++i) {
		if (path[i] == '\\' || path[i] == '/') {
			lastSepIndex = i;
		}
	}
	lastSepIndex++;

	int fnameLen = pathLen - lastSepIndex;
	char* fileName = malloc((fnameLen + 1) * sizeof(char));
	memset(fileName, '\0', fnameLen + 1);
	strncpy(fileName, path + lastSepIndex, pathLen - lastSepIndex);

	return fileName;
}
