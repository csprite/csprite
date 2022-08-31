#ifndef HELPERS_H
#define HELPERS_H

#include "cconfig.h"
#include "system.h"

static char* getPaletteDirPath() {
	char* configdir = CCGetConfigDir();
	static char configPath[CC_PATH_SIZE_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_error("cannot get the config directory!");
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "palettes");
			sys_make_dir(configPath);
		} else {
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "%s%ccsprite%cpalettes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			sys_make_dir(configPath);
		}
	}

	return configPath;
}

#endif
