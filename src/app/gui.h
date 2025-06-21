#ifndef SRC_APP_GUI_H
#define SRC_APP_GUI_H 1
#pragma once

#include "os/os.h"
#include "base/types.h"

void gui_init(OS_Handle window);
void gui_begin_frame(OS_Handle window);
void gui_end_frame(OS_Handle window);
void gui_release(OS_Handle window);

#endif // SRC_APP_GUI_H
