#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

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

int sys_make_dir(const char *path) {
	char tmp[PATH_MAX];
	char *p;
	strcpy(tmp, path);
	for (p = tmp + 1; *p; p++) {
		if (*p != '/') continue;
		*p = '\0';
#ifdef WIN32 // On Windows mkdir only takes path
		if (mkdir(tmp) != 0) {
#else
		if (mkdir(tmp, S_IRWXU) != 0) {
#endif
			if (errno != EEXIST) {
				return -1;
			}
		}
		*p = '/';
	}
	return 0;
}

