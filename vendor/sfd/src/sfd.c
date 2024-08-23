/*
 * Copyright (c) 2017 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "sfd.h"

static const char* last_error;
static int next_filter(char* dst, const char** p);

#ifdef SFD_BACKEND_WIN32

#include <stdio.h>
#include <windows.h>

typedef struct {
	unsigned long process_id;
	void* handle_root;
	void* handle_first;
} FindMainWindowInfo;

static BOOL CALLBACK _FindMainWin_CB(HWND handle, LPARAM lParam) {
	FindMainWindowInfo* info = (FindMainWindowInfo*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (info->process_id == process_id) {
		info->handle_first = handle;
		if (GetWindow(handle, GW_OWNER) == 0 && IsWindowVisible(handle)) {
			info->handle_root = handle;
			return 0;
		}
	}
	return 1;
}

static HWND _FindMainWin() {
	FindMainWindowInfo info = {
		.process_id = GetCurrentProcessId()
	};
	EnumWindows(_FindMainWin_CB, (LPARAM)&info);
	return info.handle_root;
}

static const char* make_filter_str(sfd_Options* opt) {
	static char buf[1024];
	int n;

	buf[0] = '\0';
	n = 0;

	if (opt->filter) {
		const char* p;
		char b[32];
		const char* name = opt->filter_name ? opt->filter_name : opt->filter;
		n += sprintf(buf + n, "%s", name) + 1;

		p = opt->filter;
		while (next_filter(b, &p)) {
			n += sprintf(buf + n, "%s;", b);
		}

		buf[++n] = '\0';
	}

	n += sprintf(buf + n, "All Files") + 1;
	n += sprintf(buf + n, "*.*");
	buf[++n] = '\0';

	return buf;
}

static inline void init_ofn(OPENFILENAME* ofn, sfd_Options* opt) {
	static char result_buf[2048];
	result_buf[0] = '\0';

	memset(ofn, 0, sizeof(*ofn));
	ofn->hwndOwner        = _FindMainWin();
	ofn->lStructSize      = sizeof(*ofn);
	ofn->lpstrFilter      = make_filter_str(opt);
	ofn->nFilterIndex     = 1;
	ofn->lpstrFile        = result_buf;
	ofn->Flags            = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn->nMaxFile         = sizeof(result_buf) - 1;
	ofn->lpstrInitialDir  = opt->path;
	ofn->lpstrTitle       = opt->title;
	ofn->lpstrDefExt      = opt->extension;
}

const char* sfd_open_dialog(sfd_Options* opt) {
	last_error = NULL;

	OPENFILENAME ofn;
	init_ofn(&ofn, opt);

	int ok = opt->save ? GetSaveFileName(&ofn) : GetOpenFileName(&ofn);

	return ok ? ofn.lpstrFile : NULL;
}

#endif // SFD_BACKEND_WIN32

#ifndef SFD_BACKEND_WIN32

// For popen()/pclose()
#define _POSIX_C_SOURCE 2
// For realpath()
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif // !SFD_BACKEND_WIN32

#ifdef SFD_BACKEND_KDIALOG

const char* sfd_open_dialog(sfd_Options* opt) {
	static char result_buf[2048];
	char buf[2048];
	char* p;
	const char *title;
	FILE *fp;
	int n, len;

	last_error = NULL;

	// fp = popen("kdialog --version", "r");
	// if (fp == NULL || pclose(fp) != 0) {
	// 	last_error = "could not open kdialog";
	// 	return NULL;
	// }

	n = sprintf(buf, "kdialog");

	if (opt->save) {
		n += sprintf(buf + n, " --getsavefilename");
	} else {
		n += sprintf(buf + n, " --getopenfilename");
	}

	if (opt->title) {
		title = opt->title;
	} else {
		title = opt->save ? "Save File" : "Open File";
	}

	n += sprintf(buf + n, " --title \"%s\"", title);

	if (opt->path && opt->path[0] != '\0') {
		n += sprintf(buf + n, " \"");
		p = realpath(opt->path, buf + n);
		if (p == NULL) {
			last_error = "call to realpath() failed";
			return NULL;
		}
		n += strlen(buf + n);
		n += sprintf(buf + n, "/\"");
	} else {
		n += sprintf(buf + n, " \".\"");
	}

	if (opt->filter) {
		char b[64];
		const char *p;
		n += sprintf(buf + n, " \"");

		if (opt->filter_name) {
			n += sprintf(buf + n, "%s (", opt->filter_name);
		}

		p = opt->filter;
		while (next_filter(b, &p)) {
			n += sprintf(buf + n, "%s ", b);
		}
		// Remove The Last Extra Space
		buf[n - 1] = '\0'; n--;

		if (opt->filter_name) {
			n += sprintf(buf + n, ")");
		}

		n += sprintf(buf + n, " | All Files (*)\"");
	} else {
		n += sprintf(buf + n, " \"All Files (*)\"");
	}


	fp = popen(buf, "r");
	len = fread(result_buf, 1, sizeof(result_buf) - 1, fp);
	pclose(fp);

	if (len > 0) {
		result_buf[len - 1] = '\0';
		if (opt->save && opt->extension && !strstr(result_buf, opt->extension)) {
			sprintf(&result_buf[len - 1], ".%s", opt->extension);
		}
		return result_buf;
	}

	return NULL;
}

#endif // SFD_BACKEND_KDIALOG

#ifdef SFD_BACKEND_ZENITY

const char* sfd_open_dialog(sfd_Options* opt) {
	static char result_buf[2048];
	char buf[2048];
	char* p;
	const char *title;
	FILE *fp;
	int n, len;

	last_error = NULL;

	fp = popen("zenity --version", "r");
	if (fp == NULL || pclose(fp) != 0) {
		last_error = "could not open zenity";
		return NULL;
	}

	n = sprintf(buf, "zenity --file-selection");

	if (opt->save) {
		n += sprintf(buf + n, " --save --confirm-overwrite");
	}

	if (opt->title) {
		title = opt->title;
	} else {
		title = opt->save ? "Save File" : "Open File";
	}

	n += sprintf(buf + n, " --title=\"%s\"", title);

	if (opt->path && opt->path[0] != '\0') {
		n += sprintf(buf + n, " --filename=\"");
		p = realpath(opt->path, buf + n);
		if (p == NULL) {
			last_error = "call to realpath() failed";
			return NULL;
		}
		n += strlen(buf + n);
		n += sprintf(buf + n, "/\"");
	}

	if (opt->filter) {
		char b[64];
		const char *p;
		n += sprintf(buf + n, " --file-filter=\"");

		if (opt->filter_name) {
			n += sprintf(buf + n, "%s | ", opt->filter_name);
		}

		p = opt->filter;
		while (next_filter(b, &p)) {
			n += sprintf(buf + n, "\"%s\" ", b);
		}

		n += sprintf(buf + n, "\"");
	}

	n += sprintf(buf + n, " --file-filter=\"All Files | *\"");


	fp = popen(buf, "r");
	len = fread(result_buf, 1, sizeof(result_buf) - 1, fp);
	pclose(fp);

	if (len > 0) {
		result_buf[len - 1] = '\0';
		if (opt->save && opt->extension && !strstr(result_buf, opt->extension)) {
			sprintf(&result_buf[len - 1], ".%s", opt->extension);
		}
		return result_buf;
	}

	return NULL;
}

#endif // SFD_BACKEND_ZENITY

const char* sfd_get_error(void) {
	const char* res = last_error;
	last_error = NULL;
	return res;
}

#include <string.h>

static int next_filter(char *dst, const char **p) {
	int len;

	*p += strspn(*p, "|");
	if (**p == '\0') {
		return 0;
	}

	len = strcspn(*p, "|");
	memcpy(dst, *p, len);
	dst[len] = '\0';
	*p += len;

	return 1;
}
