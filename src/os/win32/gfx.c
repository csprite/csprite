#ifdef TARGET_WINDOWS

#include "os/gfx.h"

#include <windows.h>
#include <shellapi.h>

void os_open_in_browser(String8 url) {
	ShellExecute(0, 0, (char*)url.str, 0, 0, SW_SHOW);
}

void os_show_message_box(OS_MessageBoxIcon icon, String8 title, String8 message) {
	long ico = MB_ICONWARNING;

	switch (icon) {
		case OS_MessageBoxIcon_Info:    ico = MB_ICONINFORMATION; break;
		case OS_MessageBoxIcon_Warning: ico = MB_ICONWARNING;     break;
		case OS_MessageBoxIcon_Error:   ico = MB_ICONERROR;       break;
	}

	MessageBox(NULL, (char*)title.str, (char*)message.str, MB_OK | ico);
}

#endif // TARGET_WINDOWS
