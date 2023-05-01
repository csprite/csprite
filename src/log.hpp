#ifndef CSP_LOG_HPP_INCLUDED_
#define CSP_LOG_HPP_INCLUDED_ 1

#include <cstdio>

#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__unix) || defined(__unix__)
	#define __LOG_CLR_RED    "\x1B[31m"
	#define __LOG_CLR_BLUE   "\x1B[34m"
	#define __LOG_CLR_YELLOW "\x1B[33m"
	#define __LOG_CLR_RESET  "\x1B[0m"
#else 
	#define __LOG_CLR_RED    ""
	#define __LOG_CLR_BLUE   ""
	#define __LOG_CLR_YELLOW ""
	#define __LOG_CLR_RESET  ""
#endif

#define LOG_I(fmt, ...) std::printf(__LOG_CLR_BLUE   "INFO: "  __LOG_CLR_RESET fmt "\n", ##__VA_ARGS__);
#define LOG_W(fmt, ...) std::printf(__LOG_CLR_YELLOW "WARN: "  __LOG_CLR_RESET fmt "\n", ##__VA_ARGS__);
#define LOG_E(fmt, ...) std::printf(__LOG_CLR_RED    "ERROR: " __LOG_CLR_RESET fmt "\n", ##__VA_ARGS__);

#endif // CSP_LOG_HPP_INCLUDED_
