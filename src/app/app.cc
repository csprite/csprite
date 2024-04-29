#include <cerrno>
#include <cstring>

#include "app/misc.hpp"
#include "log/log.h"

#include "fs/fs.hpp"
#include "app/fswrapper.hpp"

bool App_Initialize(void) {
	/* Enable VT100 Mode For Logging */
	EnableVT100();

	/* Ensure Required Directories Exist */
	if (!FileSystem::MakeDirRecursive(FileSystem::GetConfigDir())) {
		log_error("FileSystem::MakeDirRecursive(...) - %s", strerror(errno));
		return false;
	}

	if (!FileSystem::MakeDir(FileSystem::GetLanguagesDir())) {
		log_error("FileSystem::MakeDir(...) - %s", strerror(errno));
		return false;
	}
	if (!FileSystem::MakeDir(FileSystem::GetPalettesDir())) {
		log_error("FileSystem::MakeDir(...) - %s", strerror(errno));
		return false;
	}

	return true;
}

