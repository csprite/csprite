#include "manager.hpp"
#include "fs/fs.hpp"
#include "assets.h"

#include <cstdio>
#include <cstring>
#include <cerrno>

bool Assets::EnsureFileSystem() {
	if (Fs::MakeDirRecursive(Fs::GetConfigDir()) != 0) {
		printf("Error: Fs::MakeDirRecursive(...) - %s\n", strerror(errno));
		return false;
	}
	if (Fs::MakeDirRecursive(Fs::GetLanguagesDir()) != 0) {
		printf("Error: Fs::MakeDirRecursive(...) - %s\n", strerror(errno));
		return false;
	}

	assets_list("assets/languages/", [](int i, const char* path) -> int {
		(void)i;
		FILE* f = fopen((Fs::GetLanguagesDir() + SYS_PATH_SEP + Fs::GetBaseName(path)).c_str(), "w");

		if (f == NULL) {
			printf("Error: fopen(...) - %s\n", strerror(errno));
			return 0;
		}

		i32 sz = 0;
		u8* res = (u8*)assets_get(path, &sz);
		if (res != NULL && sz > 0) {
			fwrite(res, 1, sz, f);
		} else {
			printf("Error: assets_get(...) - returned NULL\n");
		}

		fclose(f);
		return 0;
	});

	return true;
}

