#ifdef TARGET_WINDOWS

#include "os/gfx.h"

#include <windows.h>
#include <shellapi.h>

OS_Handle os_window_init(U64 width, U64 height, String8 title) {
	WNDCLASSEXW wc = {
		sizeof(wc),
		CS_OWNDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		L"csprite (Remember To Make This Dynamic)",
		NULL
	};
	RegisterClassExW(&wc);
	HWND hwnd = CreateWindowW(wc.lpszClassName, L"Dear ImGui Win32+OpenGL3 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
}

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
