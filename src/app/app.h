#ifndef CSPRITE_SRC_APP_APP_H_INCLUDED_
#define CSPRITE_SRC_APP_APP_H_INCLUDED_
#pragma once

int app_init(void);
int app_main_loop(void);
void app_destroy(void);

void app_open_url(const char* url);

#endif
