#include "os/gfx.h"
#include "base/math.h"
#include "base/types.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef TARGET_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

void os_show_message_box(OS_MessageBoxIcon icon, String8 title, String8 message) {
	Display* dpy = XOpenDisplay(NULL);
	if (!dpy) return;

	// Load The First Available Font From The Array
	const char* FontNames[] = { "9x15", "8x16", "8x13", "7x14", "7x13", "6x13", "6x12", "6x10", "6x9", "5x8", "5x7", "4x6" };
	XFontStruct *fontInfo = NULL;
	for (unsigned long i = 0; i < ArrayCount(FontNames); i++) {
		fontInfo = XLoadQueryFont(dpy, FontNames[i]);
		if (fontInfo) {
			break;
		}
	}

	// If No Font Could Be Loaded
	if (!fontInfo) return;

	// Calculate Sizes & Positions
	const int PadX = 30, PadY = 25, LineSpacing = 5;
	const int TextHeight = fontInfo->ascent + fontInfo->descent; // Common Text Height

	String8 iconText;
	switch (icon) {
		case OS_MessageBoxIcon_Info:    iconText = str8_lit("Info"); break;
		case OS_MessageBoxIcon_Warning: iconText = str8_lit("Warning"); break;
		case OS_MessageBoxIcon_Error:   iconText = str8_lit("Error"); break;
		default:                        iconText = str8_lit("Message"); break;
	}

	;
	const int IconTextWidth = iconText.size * fontInfo->per_char->width;
	const int MessageTextWidth = message.size * fontInfo->per_char->width;

	Point IconTextPos = { .y = fontInfo->ascent + (PadY / 2) };
	Point MessageTextPos = { .y = IconTextPos.y + LineSpacing + TextHeight };

	const Rect WindowDim = {
		.w = Max(IconTextWidth, MessageTextWidth) + (PadX / 2),
		.h = MessageTextPos.y + fontInfo->descent + (PadY / 2)
	};

	IconTextPos.x = (WindowDim.w / 2) - (IconTextWidth / 2);
	MessageTextPos.x = (WindowDim.w / 2) - (MessageTextWidth / 2);

	// Create Window
	const int s = XDefaultScreen(dpy);
	Screen* scr = XScreenOfDisplay(dpy, s);
	Window w = XCreateSimpleWindow(
		dpy, XRootWindowOfScreen(scr),
		(WidthOfScreen(scr) / 2) - (WindowDim.w / 2), // Center Window Horizontally
		(HeightOfScreen(scr) / 2) - (WindowDim.h / 2), // Center Window Vertically
		WindowDim.w, WindowDim.h, 0, 0, WhitePixel(dpy, s)
	);

	// Create GC
	XGCValues gcValues = {0};
	gcValues.font = fontInfo->fid;
	gcValues.foreground = BlackPixel(dpy, s);
	GC gc = XCreateGC(dpy, w, GCFont + GCForeground, &gcValues);

	// Select Inputs
	XSelectInput(dpy, w, ExposureMask);

	// Window Title
	XStoreName(dpy, w, (char*)title.str);

	// Disable Resizing & Hint To Center The Window
	XSizeHints hints = {0};
	hints.flags       = PSize | PMinSize | PMaxSize | PWinGravity;
	hints.min_width   = hints.max_width  = hints.base_width  = WindowDim.w;
	hints.min_height  = hints.max_height = hints.base_height = WindowDim.h;
	hints.win_gravity = CenterGravity;
	XSetWMNormalHints(dpy, w, &hints);

	// Set Window Type
	Atom type = XInternAtom(dpy,"_NET_WM_WINDOW_TYPE", False);
	Atom value = XInternAtom(dpy,"_NET_WM_WINDOW_TYPE_DIALOG", False);
	XChangeProperty(dpy, w, type, XA_ATOM, 32, PropModeReplace, (unsigned char*)&value, 1);

	// Delete Window Property
	Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, w, &wmDeleteMessage, 1);

	// Show & Raise The Window
	XMapRaised(dpy, w);

	XEvent ev;
	while (1) {
		XNextEvent(dpy, &ev);
		if (ev.type == Expose) {
			XDrawString(
				dpy, w, gc,
				IconTextPos.x, IconTextPos.y,
				(char*)iconText.str, iconText.size
			);
			XDrawString(
				dpy, w, gc,
				MessageTextPos.x, MessageTextPos.y,
				(char*)message.str, message.size
			);
		} else if (ev.type == DestroyNotify || (ev.type == ClientMessage && (Atom)ev.xclient.data.l[0] == wmDeleteMessage)) {
			break;
		}
	}

	XFreeFont(dpy, fontInfo);
	XFreeGC(dpy, gc);
	XDestroyWindow(dpy, w);
	XCloseDisplay(dpy);
}
#elif defined(TARGET_WINDOWS)

#include <windows.h>
#include <shellapi.h>

void os_show_message_box(OS_MessageBoxIcon icon, String8 title, String8 message) {
	long ico = MB_ICONWARNING;

	switch (icon) {
		case OS_MessageBoxIcon_Info:    ico = MB_ICONINFORMATION; break;
		case OS_MessageBoxIcon_Warning: ico = MB_ICONWARNING;     break;
		case OS_MessageBoxIcon_Error:   ico = MB_ICONERROR;       break;
	}

	MessageBox(NULL, (char*)title.str, (char*)message.str, MB_OK | ico);
}
#else
	#error No Implementation Found!
#endif

void os_open_in_browser(String8 url) {
#if defined(TARGET_WINDOWS)
	ShellExecute(0, 0, (char*)url.str, 0, 0, SW_SHOW);
#elif defined(TARGET_APPLE) || defined(TARGET_LINUX)
	String8 executable = 
	#ifdef TARGET_LINUX
		str8_lit("setsid xdg-open");
	#else
		str8_lit("open");
	#endif
	S64 len = url.size + executable.size + str8_lit(" \"\"").size + 1;
	char* cmd = malloc(len);
	S32 ret = snprintf(cmd, len, "%s \"%s\"", executable.str, url.str);
	if (ret > 0 && ret < len) {
		system(cmd);
	}
	free(cmd);
#else
	#error No Implementation Found!
#endif
}

#include <GLFW/glfw3.h>
#include "log/log.h"

void _glfwErrCallback(int error, const char *desc) {
	log_error("GLFW Error: %d - %s", error, desc);
}

OS_Handle os_window_init(U64 width, U64 height, String8 title) {
	glfwInit();
	glfwSetErrorCallback(_glfwErrCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(width, height, (char*)title.str, NULL, NULL);

	if (window == NULL) {
		os_abort_with_message(1, str8_lit("Failed to create GLFW window!"));
	}

	OS_Handle w = {
		.value = (U64)window
	};
	return w;
}

void os_window_set_title(OS_Handle w, String8 title) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwSetWindowTitle(window, (char*)title.str);
}

B32 os_window_should_close(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	return glfwWindowShouldClose(window);
}

void os_window_show(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwShowWindow(window);
}

void os_window_release(OS_Handle w) {
	GLFWwindow* window = (GLFWwindow*)w.value;
	glfwDestroyWindow(window);
	glfwTerminate();
}
