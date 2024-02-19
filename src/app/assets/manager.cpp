#include "types.hpp"
#include "manager.hpp"
#include "fs/fs.hpp"
#include "app/fswrapper.hpp"
#include "assets.h"
#include "log/log.h"

#include <cstdio>
#include <cstring>
#include <cerrno>

namespace Fs = FileSystem;

bool Assets::EnsureFileSystem() {
	if (!Fs::MakeDirRecursive(Fs::GetConfigDir())) {
		log_error("Fs::MakeDirRecursive(...) - %s", strerror(errno));
		return false;
	}

	if (!Fs::MakeDir(Fs::GetLanguagesDir())) {
		log_error("Fs::MakeDir(...) - %s", strerror(errno));
		return false;
	}
	if (!Fs::MakeDir(Fs::GetPalettesDir())) {
		log_error("Fs::MakeDir(...) - %s", strerror(errno));
		return false;
	}

	return true;
}

