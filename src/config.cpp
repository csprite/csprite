#include <algorithm>
#include <cstdio>

#include "utils.h"
#include "log/log.h"

#include "system.h"
#include "ini/ini.h"
#include "config.hpp"

AppConfig& AppConfig::operator = (const AppConfig& rhs) {
	FramesPerSecond = rhs.FramesPerSecond;
	ThemeName = rhs.ThemeName;
	CheckerboardColor1 = rhs.CheckerboardColor1;
	CheckerboardColor2 = rhs.CheckerboardColor1;
	return *this;
}

inline bool AppConfig::operator == (const AppConfig& rhs) const {
	return (
		FramesPerSecond == rhs.FramesPerSecond &&
		ThemeName == rhs.ThemeName &&
		CheckerboardColor1 == rhs.CheckerboardColor1 &&
		CheckerboardColor2 == rhs.CheckerboardColor2
	);
}

inline bool AppConfig::operator != (const AppConfig& rhs) const {
	return !(*this == rhs);
}

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

AppConfig* LoadConfig(void) {
	AppConfig* c = new AppConfig;
	char* configPath = getSettingsPath();
	if (Sys_IsRegularFile(configPath) == 1) {
		ini_t* config = ini_load(configPath);

		const char* Max_FPS_Str = ini_get(config, "csprite", "Max_FPS");
		c->FramesPerSecond = Max_FPS_Str == NULL ? 60 : atoi(Max_FPS_Str);
		c->FramesPerSecond = c->FramesPerSecond < 5 ? 5 : c->FramesPerSecond;

		const char* RenderDriver_Str = ini_get(config, "csprite", "RenderDriver");
		c->RenderDriver = R_StringToRendererApi(RenderDriver_Str == NULL ? R_RendererApiToString(R_API_OPENGL) : RenderDriver_Str);

		const char* Theme_Name_Str = ini_get(config, "theme", "name");
		c->ThemeName = Theme_Name_Str == NULL ? "Noice Blue" : std::string(Theme_Name_Str);

		const char* CheckerColor1_Str = ini_get(config, "colors", "CheckerColor1");
		unsigned int CheckerColor1Int[4] = { 0xC0, 0xC0, 0xC0 };
		if (CheckerColor1_Str != NULL && sscanf(
			CheckerColor1_Str[0] == '#' ? CheckerColor1_Str + 1 : CheckerColor1_Str,
			"%02x%02x%02x",
			&CheckerColor1Int[0],
			&CheckerColor1Int[1],
			&CheckerColor1Int[2]) != 3) {}

		c->CheckerboardColor1 = {
			(u8)std::clamp(CheckerColor1Int[0], 0U, 255U),
			(u8)std::clamp(CheckerColor1Int[1], 0U, 255U),
			(u8)std::clamp(CheckerColor1Int[2], 0U, 255U),
			255
		};

		const char* CheckerColor2_Str = ini_get(config, "colors", "CheckerColor2");
		unsigned int CheckerColor2Int[4] = { 0x80, 0x80, 0x80 };
		if (CheckerColor2_Str != NULL && sscanf(
			CheckerColor2_Str[0] == '#' ? CheckerColor2_Str + 1 : CheckerColor2_Str,
			"%02x%02x%02x",
			&CheckerColor2Int[0],
			&CheckerColor2Int[1],
			&CheckerColor2Int[2]) != 3) {}

		c->CheckerboardColor1 = {
			(u8)std::clamp(CheckerColor2Int[0], 0U, 255U),
			(u8)std::clamp(CheckerColor2Int[1], 0U, 255U),
			(u8)std::clamp(CheckerColor2Int[2], 0U, 255U),
			255
		};

		ini_free(config);
		config = NULL;
		return c;
	} else {
		log_error("failed to open \"%s\"!", configPath);
		c->CheckerboardColor1 = { 0xC0, 0xC0, 0xC0, 0xFF };
		c->CheckerboardColor2 = { 0x80, 0x80, 0x80, 0xFF};
		c->FramesPerSecond = 60;
		c->RenderDriver = R_API_OPENGL;
		c->ThemeName = "Noice Blue";
		WriteConfig(c);
		return c;
	}

	delete c;
	return NULL;
}

int WriteConfig(AppConfig* s) {
	if (s == NULL) {
		log_error("NULL pointer passed!\n");
		return -1;
	}

	char* configPath = getSettingsPath();
	if (configPath == NULL) {
		log_error("getSettingsPath returned NULL pointer\n");
		return -1;
	}

	s->FramesPerSecond = s->FramesPerSecond < 5 ? 5 : s->FramesPerSecond;

	FILE* f = fopen(configPath, "w");
	fprintf(
		f, "[csprite]\nMax_FPS = %d\nRenderDriver = %s\n\n[theme]\nname = %s\n\n[colors]\nCheckerColor1 = %02X%02X%02X\nCheckerColor2 = %02X%02X%02X\n",
		s->FramesPerSecond,
		R_RendererApiToString(s->RenderDriver).c_str(),
		s->ThemeName.c_str(),
		s->CheckerboardColor1.r, s->CheckerboardColor1.g, s->CheckerboardColor1.b,
		s->CheckerboardColor2.r, s->CheckerboardColor2.g, s->CheckerboardColor2.b
	);
	fclose(f);
	f = NULL;

	return 0;
}
