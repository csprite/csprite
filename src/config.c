#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "config.h"
#include "system.h"
#include "ini/ini.h"
#include "logger.h"

static char* getSettingsPath() {
	char* configdir = Sys_GetConfigDir();
	static char configPath[SYS_PATHNAME_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			Logger_Error("cannot get the config directory!\n");
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "config.ini");
		} else {
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "%s/csprite", configdir);
			struct stat st = {0};
			if (stat(configPath, &st) == -1) Sys_MakeDirRecursive(configPath);
			memset(configPath, 0, sizeof(configPath));
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "%s/csprite/config.ini", configdir);
		}

		return configPath;
	}

	return configPath;
}

Config_T* LoadConfig(void) {
	Config_T* s = malloc(sizeof(Config_T));
	char* configPath = getSettingsPath();
	if (access(configPath, F_OK) == 0) {
		ini_t *config = ini_load(configPath);
		const char* vsync = ini_get(config, "csprite", "vsync");
		if (vsync == NULL)
			s->vsync = true;
		else if (strncmp(vsync, "true", 5) == 0)
			s->vsync = true;
		else
			s->vsync = false;

		const char* EventsUpdateRate_Str = ini_get(config, "csprite", "EventsUpdateRate");
		s->EventsUpdateRate = EventsUpdateRate_Str == NULL ? 70 : atoi(EventsUpdateRate_Str);
		s->EventsUpdateRate = s->EventsUpdateRate < 5 ? 5 : s->EventsUpdateRate;
		const char* FramesUpdateRate_Str = ini_get(config, "csprite", "FramesUpdateRate");
		s->FramesUpdateRate = FramesUpdateRate_Str == NULL ? 30 : atoi(FramesUpdateRate_Str);
		s->FramesUpdateRate = s->FramesUpdateRate < 5 ? 5 : s->FramesUpdateRate;

		ini_free(config);
		config = NULL;
		return s;
	} else {
		s->vsync = true;
		s->EventsUpdateRate = 70;
		s->FramesUpdateRate = 30;
		WriteConfig(s);
		return s;
	}

	return NULL;
}

int WriteConfig(Config_T* s) {
	if (s == NULL) {
		Logger_Error("NULL pointer passed!\n");
		return -1;
	}

	char* configPath = getSettingsPath();
	if (configPath == NULL) {
		Logger_Error("getSettingsPath returned NULL pointer\n");
		return -1;
	}

	s->EventsUpdateRate = s->EventsUpdateRate < 5 ? 5 : s->EventsUpdateRate;
	s->FramesUpdateRate = s->FramesUpdateRate < 5 ? 5 : s->FramesUpdateRate;

	FILE* f = fopen(configPath, "w");
	fprintf(
		f, "[csprite]\nvsync = %s\nEventsUpdateRate = %d\nFramesUpdateRate = %d\n",
		s->vsync == true ? "true" : "false",
		s->EventsUpdateRate,
		s->FramesUpdateRate
	);
	fclose(f);
	f = NULL;

	return 0;
}
