#include <stdlib.h>

#include "theme.h"
#include "ini/ini.h"
#include "system.h"
#include "macros.h"

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

theme_t* LoadTheme(const char* themeIni) {
	if (themeIni == NULL) {
		return NULL;
	}

	theme_t* t = malloc(sizeof(theme_t));
	ini_t *config = ini_load_txt(themeIni);

	t->PopupBG = str2rgbaint(ini_get(config, "theme", "PopupBG"));
	t->WindowBG = str2rgbaint(ini_get(config, "theme", "WindowBG"));
	t->Header = str2rgbaint(ini_get(config, "theme", "Header"));
	t->Header_Hovered = str2rgbaint(ini_get(config, "theme", "Header_Hovered"));
	t->Text = str2rgbaint(ini_get(config, "theme", "Text"));
	t->Text_Disabled = str2rgbaint(ini_get(config, "theme", "Text_Disabled"));
	t->Button = str2rgbaint(ini_get(config, "theme", "Button"));
	t->Button_Hovered = str2rgbaint(ini_get(config, "theme", "Button_Hovered"));
	t->FrameBG = str2rgbaint(ini_get(config, "theme", "FrameBG"));
	t->FrameBG_Hovered = str2rgbaint(ini_get(config, "theme", "FrameBG_Hovered"));
	t->TitlebarBG = str2rgbaint(ini_get(config, "theme", "TitlebarBG"));
	t->TitlebarBG_Active = str2rgbaint(ini_get(config, "theme", "TitlebarBG_Active"));
	t->Border = str2rgbaint(ini_get(config, "theme", "Border"));
	t->MenuBarBG = str2rgbaint(ini_get(config, "theme", "MenuBarBG"));
	t->Checkmark = str2rgbaint(ini_get(config, "theme", "Checkmark"));
	t->ModalDimming = str2rgbaint(ini_get(config, "theme", "ModalDimming"));

	ini_free(config);
	config = NULL;
	return t;
}
