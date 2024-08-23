#ifndef CSPRITE_APP_WINDOW_WINDOW_H_INCLUDED_
#define CSPRITE_APP_WINDOW_WINDOW_H_INCLUDED_
#pragma once

int  WindowCreate(const char *title, int width, int height, int resizable);
void WindowDestroy(void);

void WindowSetIcon(int width, int height, unsigned char *pixels);
void WindowSetTitle(const char *title);
void WindowSetBG(unsigned char r, unsigned char g, unsigned char b);
void WindowSetMaxFPS(int fps);

int  WindowShouldClose();
void WindowNewFrame();
void WindowEndFrame();

void WindowMinimize(void);
void WindowRestore(void);
void WindowMaximize(void);

#endif
