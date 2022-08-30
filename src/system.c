#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "system.h"

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#if defined(__unix__) && !defined(ANDROID)
	#include <pwd.h>
#endif

#include <stdio.h>

long int fsize(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	long int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

int sys_list_dir(const char *dirpath, int (*callback)(const char *dirpath, const char *name, void* data), void* data) {
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

int sys_make_dir(const char *dir) {
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
