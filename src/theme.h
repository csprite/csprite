#pragma once

#include "imgui.h"

#define THEME_NAME_SIZE_MAX 512
#define THEME_AUTHOR_SIZE_MAX 512 + 17 // The Extra 17 Are For "Awesome Theme By ", so the actual name starts after 17 Characters

typedef struct {
	char name[THEME_NAME_SIZE_MAX];
	char author[THEME_AUTHOR_SIZE_MAX];
	ImGuiStyle style;
} theme_t;

typedef struct {
	unsigned int numOfEntries;
	theme_t** entries;
	ImGuiStyle originalStyle;
} theme_arr_t;

int FreeTheme(theme_t* theme);
int FreeThemeArr(theme_arr_t* theme);
theme_t* LoadTheme(const char* themeIni, const char* fileName);
theme_arr_t* ThemeLoadAll();
