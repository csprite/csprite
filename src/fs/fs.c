#include "fs/fs.h"

#define _DEFAULT_SOURCE
#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

dir_t fs_list_dir_start(const char* path) {
	DIR* dir = opendir(path);
	return dir;
}

int fs_list_dir(dir_t dir, char** name, int* isDir) {
tryAgain:
	errno = 0;
	struct dirent* ent = readdir(dir);
	if (ent == NULL) {
		if (errno != 0) { // on error
			fs_list_dirEnd(dir);
			return 1;
		} else {
			*name = NULL;
		}
	} else if (ent->d_name[0] == '.') {
		goto tryAgain; // skip '.' && '..' entry
	} else {
		*name = ent->d_name;
		*isDir = ent->d_type == DT_DIR;
	}

	return 0;
}

void fs_list_dirRewind(dir_t dir) {
	rewinddir(dir);
}

void fs_list_dirEnd(dir_t dir) {
	if (dir) {
		closedir(dir);
	}
}

#include <string.h>

int fs_get_basename(const char* path) {
	int len = strlen(path);
	int index = 0;
	for (int i = 0; i < len; i++) {
		if (
			(path[i] == '/' || path[i] == '\\') &&
			i + 1 < len &&
			(path[i + 1] != '/' && path[i + 1] != '\\')
		) {
			index = i + 1;
		}
	}
	return index;
}

int fs_get_parent_dir(const char* path) {
	int len = strlen(path);
	// using - 2 since it will filter out any trailing path separator
	for (int i = len - 2; i >= 0; --i) {
		if (path[i] == '/' || path[i] == '\\') {
			return i;
		}
	}

	return -1;
}

int fs_get_extension(const char* filePath) {
	int len = strlen(filePath);

	for (int i = len - 1; i >= 0; i--) {
		if (filePath[i] == '/' || filePath[i] == '\\') {
			return -1;
		}
		if (filePath[i] == '.') {
			return i;
		}
	}

	return -1;
}
