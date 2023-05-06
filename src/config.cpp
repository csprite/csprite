#include <stdio.h>

#include "utils.h"
#include "config.h"
#include "system.h"
#include "ini/ini.h"
#include "log/log.h"

static char* getSettingsPath() {
	char* configdir = Sys_GetConfigDir();
	static char configPath[SYS_PATHNAME_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_error("cannot get the config directory!\n");
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "config.ini");
		} else {
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "%s/csprite", configdir);
			if (Sys_IsRegularDir(configPath) != 0) Sys_MakeDirRecursive(configPath);
			memset(configPath, 0, sizeof(configPath));
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "%s/csprite/config.ini", configdir);
		}

		return configPath;
	}

	return configPath;
}

Config_T* LoadConfig(void) {
	Config_T* c = new Config_T;
	char* configPath = getSettingsPath();
	if (Sys_IsRegularFile(configPath) == 0) {
		ini_t* config = ini_load(configPath);

		const char* Max_FPS_Str = ini_get(config, "csprite", "Max_FPS");
		c->Max_FPS = Max_FPS_Str == NULL ? 60 : atoi(Max_FPS_Str);
		c->Max_FPS = c->Max_FPS < 5 ? 5 : c->Max_FPS;

		const char* RenderDriver_Str = ini_get(config, "csprite", "RenderDriver");
		c->RenderDriver = R_StringToRendererApi(RenderDriver_Str == NULL ? R_RendererApiToString(R_API_OPENGL) : RenderDriver_Str);

		const char* Theme_Name_Str = ini_get(config, "theme", "name");
		c->Theme_Name = Theme_Name_Str == NULL ? "Noice Blue" : std::string(Theme_Name_Str);

		const char* CheckerColor1_Str = ini_get(config, "colors", "CheckerColor1");
		unsigned int CheckerColor1Int[4] = { 0xC0, 0xC0, 0xC0 };
		if (CheckerColor1_Str != NULL && sscanf(
			CheckerColor1_Str[0] == '#' ? CheckerColor1_Str + 1 : CheckerColor1_Str,
			"%02x%02x%02x",
			&CheckerColor1Int[0],
			&CheckerColor1Int[1],
			&CheckerColor1Int[2]) != 3) {}

		c->CheckerboardColor1[0] = CLAMP_NUM(CheckerColor1Int[0], 0, 255);
		c->CheckerboardColor1[1] = CLAMP_NUM(CheckerColor1Int[1], 0, 255);
		c->CheckerboardColor1[2] = CLAMP_NUM(CheckerColor1Int[2], 0, 255);

		const char* CheckerColor2_Str = ini_get(config, "colors", "CheckerColor2");
		unsigned int CheckerColor2Int[4] = { 0x80, 0x80, 0x80 };
		if (CheckerColor2_Str != NULL && sscanf(
			CheckerColor2_Str[0] == '#' ? CheckerColor2_Str + 1 : CheckerColor2_Str,
			"%02x%02x%02x",
			&CheckerColor2Int[0],
			&CheckerColor2Int[1],
			&CheckerColor2Int[2]) != 3) {}

		c->CheckerboardColor2[0] = CLAMP_NUM(CheckerColor2Int[0], 0, 255);
		c->CheckerboardColor2[1] = CLAMP_NUM(CheckerColor2Int[1], 0, 255);
		c->CheckerboardColor2[2] = CLAMP_NUM(CheckerColor2Int[2], 0, 255);

		ini_free(config);
		config = NULL;
		return c;
	} else {
		c->Max_FPS = 60;
		c->RenderDriver = R_API_OPENGL;
		c->Theme_Name = "Noice Blue";
		WriteConfig(c);
		return c;
	}

	delete c;
	return NULL;
}

int WriteConfig(Config_T* s) {
	if (s == NULL) {
		log_error("NULL pointer passed!\n");
		return -1;
	}

	char* configPath = getSettingsPath();
	if (configPath == NULL) {
		log_error("getSettingsPath returned NULL pointer\n");
		return -1;
	}

	s->Max_FPS = s->Max_FPS < 5 ? 5 : s->Max_FPS;

	FILE* f = fopen(configPath, "w");
	fprintf(
		f, "[csprite]\nMax_FPS = %d\nRenderDriver = %s\n\n[theme]\nname = %s\n\n[colors]\nCheckerColor1 = %02X%02X%02X\nCheckerColor2 = %02X%02X%02X\n",
		s->Max_FPS,
		R_RendererApiToString(s->RenderDriver).c_str(),
		s->Theme_Name.c_str(),
		s->CheckerboardColor1[0], s->CheckerboardColor1[1], s->CheckerboardColor1[2],
		s->CheckerboardColor2[0], s->CheckerboardColor2[1], s->CheckerboardColor2[2]
	);
	fclose(f);
	f = NULL;

	return 0;
}
