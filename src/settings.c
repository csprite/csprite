#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "settings.h"
#include "cconfig.h"
#include "log/log.h"
#include "ini/ini.h"

// https://stackoverflow.com/a/42906151/14516016
#if defined(_WIN32) || defined(WIN32)
	#include <windows.h>
	#include <direct.h>
	#define mkdir(dir, mode) _mkdir(dir)
#endif

static char* getSettingsPath() {
	char* configdir = CCGetConfigDir();
	static char configPath[CC_PATH_SIZE_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_error("cannot get the config directory!");
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "settings.ini");
		} else {
			// configPath = strncpy(configPath, configdir, CC_PATH_SIZE_MAX);
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "%s/csprite", configdir);
			struct stat st = {0};
			if (stat(configPath, &st) == -1) {
				mkdir(configPath, 0700);
			}
			memset(configPath, 0, sizeof(configPath));
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "%s/csprite/settings.ini", configdir);
		}

		return configPath;
	}

	return configPath;
}

settings_t* LoadSettings(void) {
	settings_t* s = malloc(sizeof(settings_t));
	char* configPath = getSettingsPath();
	if (access(configPath, F_OK) == 0) {
		ini_t *config = ini_load(configPath);
		const char* vsync = ini_get(config, "csprite", "vsync");
		const char* accelerated = ini_get(config, "csprite", "accelerated");
		const char* renderer = ini_get(config, "csprite", "renderer");
		const char* customCursor = ini_get(config, "csprite", "CustomCursor");
		if (vsync == NULL)
			s->vsync = true;
		else if (strncmp(vsync, "true", 5) == 0)
			s->vsync = true;
		else
			s->vsync = false;

		if (customCursor == NULL)
			s->CustomCursor = true;
		else if (strncmp(customCursor, "true", 5) == 0)
			s->CustomCursor = true;
		else
			s->CustomCursor = false;

		if (renderer == NULL)
			strncpy(s->renderer, "software", 128);
		else {
			strncpy(s->renderer, renderer, 128);
			for(int i = 0; s->renderer[i]; i++){
				s->renderer[i] = tolower(s->renderer[i]);
			}
		}

		if (accelerated == NULL)
			s->accelerated = true;
		else if (strncmp(accelerated, "true", 5) == 0)
			s->accelerated = true;
		else
			s->accelerated = false;

		ini_free(config);
		config = NULL;
		return s;
	} else {
		s->vsync = true;

	#if defined(__APPLE__)
		strncpy(s->renderer, "metal", 128);
	#elif defined(__linux__) || defined(__unix__)
		strncpy(s->renderer, "opengl", 128);
	#elif defined(_WIN32) || defined(WIN32)
		strncpy(s->renderer, "direct3d", 128);
	#else
		strncpy(s->renderer, "software", 128);
	#endif

		WriteSettings(s);
		return s;
	}

	return NULL;
}

int WriteSettings(settings_t* s) {
	if (s == NULL) {
		log_error("NULL pointer passed!");
		return -1;
	}

	char* configPath = getSettingsPath();
	if (configPath == NULL) {
		log_error("getSettingsPath returned NULL pointer");
		return -1;
	}

	// Convert String To LowerCase
	for(int i = 0; s->renderer[i]; i++){
		s->renderer[i] = tolower(s->renderer[i]);
	}

	FILE* f = fopen(configPath, "w");
	fprintf(
		f, "[csprite]\nvsync = %s\nrenderer = %s\naccelerated = %s\nCustomCursor = %s\n",
		s->vsync == true ? "true" : "false",
		s->renderer,
		s->accelerated == true ? "true" : "false",
		s->CustomCursor == true ? "true" : "false"
	);
	fclose(f);
	f = NULL;

	return 0;
}