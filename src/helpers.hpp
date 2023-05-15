#ifndef CSP_HELPERS_HPP_INCLUDE_
#define CSP_HELPERS_HPP_INCLUDE_
#pragma once

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <string>

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

void logGLFWErrors(int error, const char *description) {
	printf("Error: %d\n%s\n", error, description);
}

void openUrl(std::string url) {
#if defined(__linux__) || defined(__FreeBSD__)
	system(("xdg-open \"" + url + "\"").c_str());
#elif defined(__APPLE__)
	system(("open \"" + url + "\"").c_str());
#elif defined(_WIN32)
	ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#endif
}

#endif // CSP_HELPERS_HPP_INCLUDE_
