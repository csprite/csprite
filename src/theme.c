#include <stdlib.h>
#include <string.h>

#include "theme.h"
#include "ini/ini.h"
#include "system.h"
#include "macros.h"
#include "system.h"
#include "logger.h"
#include "assets.h"

static int OnSysDirList(const char *dir, const char *fname, void* data);
static int OnAssetMgrList(int i, const char *fname);

static Uint32 str2rgbaint(const char* str) {
	if (str == NULL) {
		printf("str in NULL!\n");
		return 0xFF0000FF;
	}

	unsigned int r = 0, g = 0, b = 0, a = 0;
	sscanf(str, "%02x%02x%02x%02x", &r, &g, &b, &a);
	return RGBA2UINT32(r, g, b, a);
}

void _PrintColor_T(Uint32 col) {
	printf("Color: %08x\n", col);
}

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

theme_t* LoadTheme(const char* themeIni) {
	if (themeIni == NULL) {
		return NULL;
	}

	theme_t* t = malloc(sizeof(theme_t));
	ini_t *config = ini_load_txt(themeIni);

	const char* name = ini_get(config, "theme", "name");
	if (name != NULL) strncpy(t->name, name, THEME_NAME_SIZE_MAX);
	else strncpy(t->name, "theme", 6);

	const char* author = ini_get(config, "theme", "author");
	memset(t->author, 0, THEME_AUTHOR_SIZE_MAX);
	snprintf(t->author, THEME_AUTHOR_SIZE_MAX, "Awesome Theme By %s", author == NULL ? "Unknown" : author);

	t->PopupBG = str2rgbaint(ini_get(config, "theme", "PopupBG"));
	t->WindowBG = str2rgbaint(ini_get(config, "theme", "WindowBG"));
	t->Header = str2rgbaint(ini_get(config, "theme", "Header"));
	t->Header_Hovered = str2rgbaint(ini_get(config, "theme", "Header_Hovered"));
	t->Text = str2rgbaint(ini_get(config, "theme", "Text"));
	t->Text_Disabled = str2rgbaint(ini_get(config, "theme", "Text_Disabled"));
	t->Button = str2rgbaint(ini_get(config, "theme", "Button"));
	t->Button_Hovered = str2rgbaint(ini_get(config, "theme", "Button_Hovered"));
	t->Button_Active = str2rgbaint(ini_get(config, "theme", "Button_Active"));
	t->FrameBG = str2rgbaint(ini_get(config, "theme", "FrameBG"));
	t->FrameBG_Hovered = str2rgbaint(ini_get(config, "theme", "FrameBG_Hovered"));
	t->TitlebarBG = str2rgbaint(ini_get(config, "theme", "TitlebarBG"));
	t->TitlebarBG_Active = str2rgbaint(ini_get(config, "theme", "TitlebarBG_Active"));
	t->Border = str2rgbaint(ini_get(config, "theme", "Border"));
	t->MenuBarBG = str2rgbaint(ini_get(config, "theme", "MenuBarBG"));
	t->Checkmark = str2rgbaint(ini_get(config, "theme", "Checkmark"));
	t->ModalDimming = str2rgbaint(ini_get(config, "theme", "ModalDimming"));
	t->TabBarBG = str2rgbaint(ini_get(config, "theme", "TabBarBG"));
	t->TabBar_Border = str2rgbaint(ini_get(config, "theme", "TabBar_Border"));

	ini_free(config);
	config = NULL;
	return t;
}

char* SysGetThemesDir() {
	char* configdir = Sys_GetConfigDir();
	static char configPath[SYS_PATH_MAX_SIZE + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			Logger_Error("cannot get the themes directory!\n");
			snprintf(configPath, SYS_PATH_MAX_SIZE + 128, "themes");
			Sys_MakeDirRecursive(configPath);
		} else {
			snprintf(configPath, SYS_PATH_MAX_SIZE + 128, "%s%ccsprite%cthemes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			Sys_MakeDirRecursive(configPath);
		}
	}

	return configPath;
}

theme_arr_t* ThemeLoadAll() {
	char* themesDirPath = SysGetThemesDir();
	char dir[SYS_PATH_MAX_SIZE + 128] = "";
	strncpy(dir, themesDirPath, SYS_PATH_MAX_SIZE);

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

	theme_arr_t* tArr = malloc(sizeof(theme_arr_t));
	tArr->numOfEntries = numOfThemes;
	tArr->entries = malloc(numOfThemes * sizeof(theme_t*));

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
			tArr->entries[i] = LoadTheme(iniTxt);

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
	char dir[SYS_PATH_MAX_SIZE + 128] = "";
	char* fileName = Sys_GetBasename(fname);
	snprintf(dir, SYS_PATH_MAX_SIZE + 128, "%s/%s", themesDirPath, fileName);

	free(fileName);
	fileName = NULL;

	data = Assets_Get(fname, NULL);
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
