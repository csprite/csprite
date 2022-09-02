#ifndef THEME_H
#define THEME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int Uint32;

typedef struct {
	char name[512];
	Uint32 PopupBG;
	Uint32 WindowBG;
	Uint32 Header;
	Uint32 Header_Hovered;
	Uint32 Text;
	Uint32 Text_Disabled;
	Uint32 Button;
	Uint32 Button_Hovered;
	Uint32 FrameBG;
	Uint32 FrameBG_Hovered;
	Uint32 TitlebarBG;
	Uint32 TitlebarBG_Active;
	Uint32 Border;
	Uint32 MenuBarBG;
	Uint32 Checkmark;
	Uint32 ModalDimming;
} theme_t;

theme_t* LoadTheme(const char* themeIni);

#ifdef __cplusplus
}
#endif

#endif // THEME_H
