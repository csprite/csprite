#include "platform/platform.h"
#include "base/memory.h"

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Platform_OpenURL(const char* URL) {
#if defined(TARGET_WINDOWS)
	ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif defined(TARGET_APPLE) || defined(TARGET_LINUX)
	long long len = strlen(URL) + 100;
	char* cmd = Memory_Alloc(len);
	int ret = snprintf(
	    cmd, len,
		#ifdef TARGET_APPLE
			"open \"%s\"",
		#else
		    "setsid xdg-open \"%s\"",
		#endif
	    URL
	);
	if (ret > 0 && ret < len) {
		system(cmd);
	}
	Memory_Dealloc(cmd);
#else
	#error No Implementation Found!
#endif
}

void Platform_Abort(U32 ExitCode) {
#if defined(TARGET_WINDOWS)
	ExitProcess(ExitCode);
#else
	exit(ExitCode);
#endif
}

void Platform_AbortMessage(U32 ExitCode, const char* Message) {
	Platform_Abort(ExitCode);
}
