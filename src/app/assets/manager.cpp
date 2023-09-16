#include "manager.hpp"
#include "fs/fs.hpp"
#include "assets.h"
#include "log/log.h"

#include <cstdio>
#include <cstring>
#include <cerrno>

bool Assets::EnsureFileSystem() {
	if (Fs::MakeDirRecursive(Fs::GetConfigDir()) != 0) {
		log_error("Fs::MakeDirRecursive(...) - %s", strerror(errno));
		return false;
	}
	if (Fs::MakeDirRecursive(Fs::GetLanguagesDir()) != 0) {
		log_error("Fs::MakeDirRecursive(...) - %s", strerror(errno));
		return false;
	}

	assets_list("assets/languages/", [](int i, const char* path) -> int {
		(void)i;
		FILE* f = fopen((Fs::GetLanguagesDir() + SYS_PATH_SEP + Fs::GetBaseName(path)).c_str(), "w");

		if (f == NULL) {
			log_error("fopen(...) - %s", strerror(errno));
			return 0;
		}

		i32 sz = 0;
		u8* res = (u8*)assets_get(path, &sz);
		if (res != NULL && sz > 0) {
			fwrite(res, 1, sz, f);
		} else {
			log_error("assets_get(...) - returned NULL for '%s'", path);
		}

		fclose(f);
		return 0;
	});

	return true;
}

