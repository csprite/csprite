#ifndef SRC_OS_GFX_H
#define SRC_OS_GFX_H 1

#include "base/string.h"

typedef enum {
	OS_MessageBoxIcon_Info,
	OS_MessageBoxIcon_Warning,
	OS_MessageBoxIcon_Error,
} OS_MessageBoxIcon;

// Misc.
void os_show_message_box(OS_MessageBoxIcon icon, String8 title, String8 message);
void os_open_in_browser(String8 url);

#endif // SRC_OS_GFX_H
