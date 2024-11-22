#ifndef CSPRITE_SRC_APP_APP_H_INCLUDED_
#define CSPRITE_SRC_APP_APP_H_INCLUDED_
#pragma once

void app_init(void);
void app_main_loop(void);
void app_destroy(void);

void app_open_url(const char* url);

#endif
