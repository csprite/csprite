#ifndef SRC_OS_GFX_H
#define SRC_OS_GFX_H 1

#include "base/string.h"
#include "os/os.h"

typedef enum {
	OS_MessageBoxIcon_Info,
	OS_MessageBoxIcon_Warning,
	OS_MessageBoxIcon_Error,
} OS_MessageBoxIcon;

// Window - Creates A Window With OpenGL Initialized
// NOTE(pegvin) - Window is hidden by default, Use `os_window_show`
OS_Handle os_window_init(U64 width, U64 height, String8 title);
void      os_window_show(OS_Handle window);
void      os_window_swap(OS_Handle window);
void      os_window_set_title(OS_Handle window, String8 title);
void      os_window_poll_events(OS_Handle window);
void*     os_window_get_native_handle(OS_Handle window);
B32       os_window_should_close(OS_Handle window);
void      os_window_release(OS_Handle window);

// Misc.
void os_show_message_box(OS_MessageBoxIcon icon, String8 title, String8 message);
void os_open_in_browser(String8 url);

#endif // SRC_OS_GFX_H
