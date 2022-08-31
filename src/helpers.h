#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include "cconfig.h"
#include "system.h"

static int DownloadFileFrom(const char* url, const char* filePath) {
	char command[2048] = "";

	// Just Download With Curl, Windows 10 Comes Installed With Curl, if user doesn't have it we can just tell them to install it.
	snprintf(command, 2048, "curl -L %s --output %s", url, filePath);
	int result = system((const char*)command);

	return result;
}

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
