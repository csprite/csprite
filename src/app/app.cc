#include <cerrno>
#include <cstring>

#include "app/app.hh"
#include "log/log.h"

#include "app/misc.hpp"

#include "fs/fs.hpp"
#include "app/fswrapper.hpp"

#include "palette/palette.hpp"
#include "app/i18n/strings.hpp"

bool App_Initialize(Preferences& prefs) {
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

	/* Load User Preferences */
	if (!prefs.Load(FileSystem::GetConfigFile().c_str())) {
		prefs = Preferences();
	}

	PaletteHelper::UpdateEntries();
	UIString::UpdateEntries();
	if (prefs.langFileName.empty() || !UIString::LoadFile(prefs.langFileName)) {
		UIString::LoadDefault();
	}

	return true;
}

