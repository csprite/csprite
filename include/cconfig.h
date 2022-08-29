/* C Config ~ get configuration directories, etc
 *
 * Copyright (c) 2022 Aditya Mishra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef CCONFIG_H
#define CCONFIG_H

#ifndef CC_PATH_SIZE_MAX
	#define CC_PATH_SIZE_MAX 2048
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
	Function: CCGetConfigDir
	Returns:
		- Windows: %APPDATA%
		- Linux: $XDG_CONFIG_HOME or $HOME/.config
		- Mac: $HOME/Library/Application Support
	Note:
		- Returned Pointer doesn't need to be freed as it's allocated & de-allocated autmatically.
*/
static char* CCGetConfigDir(void);

#ifdef __cplusplus
}
#endif

#if defined(__unix__) || defined(__linux__)

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>

// Uses $XDG_CONFIG_HOME or $HOME/.config, env variables
static char* CCGetConfigDir(void) {
	static char fullPath[CC_PATH_SIZE_MAX] = "";
	const char* configHome = NULL;

	configHome = getenv("XDG_CONFIG_HOME");
	if (!configHome) {
		configHome = getenv("HOME");

		if (!configHome) {
			configHome = getpwuid(getuid())->pw_dir;
			if (!configHome) return NULL;
		}

		snprintf(fullPath, CC_PATH_SIZE_MAX, "%s/.config", configHome);
	} else {
		snprintf(fullPath, CC_PATH_SIZE_MAX, "%s", configHome);
	}

	return fullPath;
}

#elif defined(_WIN32) || defined(WIN32)

#include <stdlib.h>

// Uses $APPDATA, env variables
static char* CCGetConfigDir(void) {
	static char fullPath[CC_PATH_SIZE_MAX] = "";
	const char* appdata = getenv("APPDATA");
	strncpy(fullPath, appdata, CC_PATH_SIZE_MAX);
	return fullPath;
}

#elif defined(__APPLE__) || defined(__MACH__)

#include <stdlib.h>
#include <string.h>

// Returns $HOME/Library/Application Support
static char* CCGetConfigDir(void) {
	static char fullPath[CC_PATH_SIZE_MAX] = "";
	const char* home = getenv("HOME");
	snprintf(fullPath, CC_PATH_SIZE_MAX, "%s/Library/Application Support", home);
	return fullPath;
}

#endif

#endif // CCONFIG_H