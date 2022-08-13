#if defined(__linux__) || defined(__FreeBSD__)
#elif defined(__APPLE__)
#elif defined(_WIN32)
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#include <windows.h>
	#include <shellapi.h>
#endif

/*
	Macro: COLOR_EQUAL(unsigned char* a, unsigned char* b)
	Description: Checks if 2 Colors RGBA are Equal
*/
#define COLOR_EQUAL(a, b)          \
	( *(a + 0) == *(b + 0)   &&    \
	  *(a + 1) == *(b + 1)   &&    \
	  *(a + 2) == *(b + 2)   &&    \
	  *(a + 3) == *(b + 3) )

/*
	Macro: OpenURL(std::string URL)
	Description: Opens The Given URL in default browser, if no implementation found logs a msg in console
	Notes: I could've made it a simple function but i saw macro was a little fast
*/
#if defined(__linux__) || defined(__FreeBSD__)
	#define OpenURL(URL) \
		system((std::string("xdg-open \"") + URL + "\"").c_str())
#elif defined(__APPLE__)
	#define OpenURL(URL) \
		system((std::string("open \"") + URL + "\"").c_str())
#elif defined(_WIN32)
	#define OpenURL(URL) \
		ShellExecute(0, 0, URL.c_str(), 0, 0, SW_SHOW)
#else
	#define OpenURL(URL) \
		printf("cannot open url: %s, because no function implementation found!", URL.c_str())
#endif
