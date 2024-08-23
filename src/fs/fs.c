#include "fs/fs.h"

#define _DEFAULT_SOURCE
#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

static DIR* dir = NULL;

int fs_list_dir(const char* dirPath, char** name, int* isDir) {
	if (dir == NULL) {
		dir = opendir(dirPath);
		if (dir == NULL) {
			return 1;
		}
	}

tryAgain:
	errno = 0;
	struct dirent* ent = readdir(dir);
	if (ent == NULL) {
		if (errno != 0) { // readdir returned NULL because error occurred
			closedir(dir);
			dir = NULL;
			return 1;
		} else { // readdir returned NULL because no more entries
			*name = NULL;
			closedir(dir);
			dir = NULL;
		}
	} else if (ent->d_name[0] == '.') {
		goto tryAgain; // skip "." & ".." entries
	} else {
		*name = ent->d_name;
		*isDir = ent->d_type == DT_DIR;
	}

	return 0;
}

void fs_list_dir_abrupt_end(void) {
	if (dir) {
		closedir(dir);
		dir = NULL;
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
