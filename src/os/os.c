#include "os/os.h"

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void os_open_url(const char* url) {
#if defined(TARGET_WINDOWS)
	ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif defined(TARGET_APPLE) || defined(TARGET_LINUX)
	long long len = strlen(url) + 100;
	char* cmd = malloc(len);
	int ret = snprintf(
	    cmd, len,
		#ifdef TARGET_APPLE
			"open \"%s\"",
		#else
		    "setsid xdg-open \"%s\"",
		#endif
	    url
	);
	if (ret > 0 && ret < len) {
		system(cmd);
	}
	free(cmd);
#else
	#error "AppOpenUrl(...) Not Implemented For Target"
#endif
}
