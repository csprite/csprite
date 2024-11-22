#ifndef CSPRITE_APP_WINDOW_WINDOW_H_INCLUDED_
#define CSPRITE_APP_WINDOW_WINDOW_H_INCLUDED_
#pragma once

void window_init(const char *title, int width, int height, int resizable);
void window_deinit(void);

void window_set_icon(int width, int height, unsigned char *pixels);
void window_set_title(const char *title);
void window_set_bg(unsigned char r, unsigned char g, unsigned char b);
void window_set_max_fps(int fps);

int  window_should_close();
void window_new_frame();
void window_end_frame();

void window_minimize(void);
void window_restore(void);
void window_maximize(void);

#endif
