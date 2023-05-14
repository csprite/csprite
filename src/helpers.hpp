#ifndef CSP_HELPERS_HPP_INCLUDE_
#define CSP_HELPERS_HPP_INCLUDE_

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <string>

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

void clampInteger(int *input, int min, int max) {
	*input = *input <= min ? min : *input;
	*input = *input >= max ? max : *input;
}

// Convert String To Integer
int string_to_int(int *out, char *s) {
	char *end;
	if (s[0] == '\0')
		return -1;
	long l = strtol(s, &end, 10);
	if (l > INT_MAX)
		return -2;
	if (l < INT_MIN)
		return -3;
	if (*end != '\0')
		return -1;
	*out = l;
	return 0;
}

// Check if 2 colors are equal
int color_equal(unsigned char *a, unsigned char *b) {
	if (*(a + 0) == *(b + 0) && *(a + 1) == *(b + 1) && *(a + 2) == *(b + 2) &&
		*(a + 3) == *(b + 3)) {
		return 1;
	}
	return 0;
}

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
