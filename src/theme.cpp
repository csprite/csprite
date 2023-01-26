#include <stdlib.h>
#include <string.h>

#include "theme.h"
#include "ini/ini.h"
#include "system.h"
#include "macros.h"
#include "system.h"
#include "logger.h"
#include "assets.h"
#include "imgooeystyles/imgui_styles.h"

static int OnSysDirList(const char *dir, const char *fname, void* data);
static int OnAssetMgrList(int i, const char *fname);

int FreeTheme(theme_t* theme) {
	if (theme == NULL) return -1;

	free(theme);
	return 0;
};

int FreeThemeArr(theme_arr_t* tArr) {
	if (tArr == NULL) {
		Logger_Error("Theme Array Pointer is NULL!\n");
		return -1;
	}

	if (tArr->entries != NULL) {
		for (int i = 0; i < tArr->numOfEntries; ++i) {
			FreeTheme(tArr->entries[i]);
			tArr->entries[i] = NULL;
		}
		free(tArr->entries);
		tArr->entries = NULL;
	}

	free(tArr);
	return 0;
};

theme_t* LoadTheme(const char* themeIni, const char* fileName) {
	if (themeIni == NULL) {
		return NULL;
	}

	theme_t* t = (theme_t*)malloc(sizeof(theme_t));
	ini_t *config = ini_load_txt(themeIni);

	const char* name = ini_get(config, "theme", "name");
	if (name != NULL) strncpy(t->name, name, THEME_NAME_SIZE_MAX - 1);
	else strncpy(t->name, "theme", 6);

	const char* author = ini_get(config, "theme", "author");
	memset(t->author, 0, THEME_AUTHOR_SIZE_MAX);
	snprintf(t->author, THEME_AUTHOR_SIZE_MAX, "Awesome Theme By %s", author == NULL ? "Unknown" : author);

	ImGui::LoadStyleFrom(fileName, t->style);

	ini_free(config);
	config = NULL;
	return t;
}

char* SysGetThemesDir() {
	char* configdir = Sys_GetConfigDir();
	static char configPath[SYS_PATHNAME_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			Logger_Error("cannot get the themes directory!\n");
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "themes");
			Sys_MakeDirRecursive(configPath);
		} else {
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "%s%ccsprite%cthemes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			Sys_MakeDirRecursive(configPath);
		}
	}

	return configPath;
}

theme_arr_t* ThemeLoadAll() {
	char* themesDirPath = SysGetThemesDir();
	char dir[SYS_PATHNAME_MAX + 128] = "";
	strncpy(dir, themesDirPath, SYS_PATHNAME_MAX);

	int numOfThemes = Sys_ListDirContents((const char*)dir, NULL, NULL);
	if (numOfThemes <= 0) {
		Sys_MakeDirRecursive(dir);
		Assets_List("data/themes/", OnAssetMgrList);
	}

	numOfThemes = Sys_ListDirContents((const char*)dir, NULL, NULL);
	if (numOfThemes <= 0) {
		Logger_Error("cannot extract the Themes!\n");
		return NULL;
	}

	theme_arr_t* tArr = (theme_arr_t*)malloc(sizeof(theme_arr_t));
	tArr->numOfEntries = numOfThemes;
	tArr->entries = (theme_t**)malloc(numOfThemes * sizeof(theme_t*));
	tArr->originalStyle = ImGui::GetStyle();

	for (int i = 0; i < numOfThemes; ++i) {
		tArr->entries[i] = NULL;
	}

	Sys_ListDirContents((const char*)dir, OnSysDirList, tArr);

	return tArr;
}

static int OnSysDirList(const char *dir, const char *fname, void* data) {
	theme_arr_t* tArr = (theme_arr_t*) data;
	int pItems = tArr->numOfEntries;

	for (int i = 0; i < pItems; ++i) {
		if (tArr->entries[i] == NULL) {
			char fullPath[2048] = "";
			snprintf(fullPath, 2048, "%s/%s", dir, fname);

			FILE* fp = fopen(fullPath, "r");
			long int size = Sys_GetFileSize(fp); // XX - Do Error (-1) Checking
			char* iniTxt = (char*) malloc((size + 1) * sizeof(char));
			memset(iniTxt, '\0', size + 1);
			fread(iniTxt, size + 1, 1, fp);
			tArr->entries[i] = LoadTheme(iniTxt, fullPath);

			free(iniTxt);
			fclose(fp);
			break;
		}
	}
	return 0;
}

static int OnAssetMgrList(int i, const char *fname) {
	FILE* file = NULL;
	const char* data = NULL;
	char* themesDirPath = SysGetThemesDir();
	char dir[SYS_PATHNAME_MAX + 128] = "";
	char* fileName = Sys_GetBasename(fname);
	snprintf(dir, SYS_PATHNAME_MAX + 128, "%s/%s", themesDirPath, fileName);

	free(fileName);
	fileName = NULL;

	data = (const char*)Assets_Get(fname, NULL);
	file = fopen(dir, "wb");
	if (file) {
		fprintf(file, "%s", data);
		fclose(file);
		file = NULL;
	} else {
		Logger_Error("cannot open file to write\n");
		return -1;
	}
	return 0;
}
