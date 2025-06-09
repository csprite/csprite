#include "fs/fs.h"

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
