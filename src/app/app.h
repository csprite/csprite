#ifndef CSPRITE_SRC_APP_APP_H_INCLUDED_
#define CSPRITE_SRC_APP_APP_H_INCLUDED_
#pragma once

#include "os/gfx.h"

OS_Handle app_init(void);
void app_tick(OS_Handle w);
void app_main_loop(OS_Handle w);
void app_release(OS_Handle w);

#endif
