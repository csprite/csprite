#ifdef TARGET_WINDOWS

#include "os/gfx.h"

#include <stdlib.h>
#include <windows.h>
#include <GL/GL.h>
#include <shellapi.h>
#include "cimgui/cimgui_impl.h"

typedef struct {
	WNDCLASSEXW wc;     // Window Class Information
	HWND        hwnd;   // Window Handle
	HGLRC       gl_ctx; // OpenGL Context
	HDC         win_dc; // Window's Device Context which is configured for RGBA32 pixel format
	B32   should_close; // Whether Window was requested to be closed
} OS_Win_GL_Handle;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return 1;
	}

	switch (msg) {
		case WM_SIZE: {
			if (wParam != SIZE_MINIMIZED) {
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);
				glViewport(0, 0, width, height);
			}
			return 0;
		}
		case WM_SYSCOMMAND: {
			// Disable ALT application menu
			if ((wParam & 0xfff0) == SC_KEYMENU) {
				return 0;
			}
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

OS_Handle os_window_init(U64 width, U64 height, String8 title) {
	WNDCLASSEXW wc = {
		sizeof(wc), CS_OWNDC, WndProc, 0L, 0L,
		GetModuleHandle(NULL),
		NULL, NULL, NULL, NULL,
		L"csprite (Remember To Make This Dynamic)", NULL
	};
	RegisterClassExW(&wc);
	HWND hWnd = CreateWindowW(
	    wc.lpszClassName, L"csprite (Remember to make this dynamic)",
	    WS_OVERLAPPEDWINDOW, 100, 100, width, height, NULL, NULL, wc.hInstance, NULL
	);

	{
		HDC win_dc = GetDC(hWnd);
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;

		const int pf = ChoosePixelFormat(win_dc, &pfd);
		if (pf == 0 || SetPixelFormat(win_dc, pf, &pfd) == FALSE) {
			wglMakeCurrent(NULL, NULL);
			ReleaseDC(hWnd, win_dc);
			os_abort_with_message(1, str8_lit("Failed To Choose/Set Pixel Format For HDC"));
		}

		ReleaseDC(hWnd, win_dc);
	}

	HDC win_dc = GetDC(hWnd);
	HGLRC gl_ctx = wglCreateContext(win_dc);
	wglMakeCurrent(win_dc, gl_ctx);

	// TODO(pegvin) - Maybe use Arenas?
	OS_Win_GL_Handle* wrapper = malloc(sizeof(OS_Win_GL_Handle));
	wrapper->wc = wc;
	wrapper->hwnd = hWnd;
	wrapper->gl_ctx = gl_ctx;
	wrapper->win_dc = win_dc;

	OS_Handle handle = { .value = wrapper };
	return handle;
}

void os_window_show(OS_Handle w) {
	OS_Win_GL_Handle* handle = (OS_Win_GL_Handle*)w->value;
	ShowWindow(handle->hwnd, SW_SHOWDEFAULT);
	UpdateWindow(handle->hwnd);
}

void os_window_swap(OS_Handle w) {
	OS_Win_GL_Handle* handle = (OS_Win_GL_Handle*)w->value;
	SwapBuffers(handle->win_dc);
}

// TODO(pegvin)
void os_window_set_title(OS_Handle window, String8 title) {
}

void os_window_poll_events(OS_Handle w) {
	OS_Win_GL_Handle* handle = (OS_Win_GL_Handle*)w->value;
	MSG msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) {
			handle->should_close = 1;
		}
	}
}

void* os_window_get_native_handle(OS_Handle w) {
	OS_Win_GL_Handle* handle = (OS_Win_GL_Handle*)w->value;
	return handle->hwnd;
}

B32 os_window_should_close(OS_Handle w) {
	OS_Win_GL_Handle* handle = (OS_Win_GL_Handle*)w->value;
	return handle->should_close;
}

void os_window_release(OS_Handle w) {
	OS_Win_GL_Handle* handle = (OS_Win_GL_Handle*)w->value;
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(handle->hwnd, handle->win_dc);
	wglDeleteContext(handle->gl_ctx);
	DestroyWindow(handle->hwnd);
	UnregisterClassW(handle->wc.lpszClassName, handle->wc.hInstance);
	free(handle);
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
