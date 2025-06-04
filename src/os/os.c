#include <stdlib.h>
#include "os/os.h"
#include "os/gfx.h"

OS_Handle os_handle_zero(void) {
	OS_Handle h = {0};
	return h;
}

B32 os_handle_match(OS_Handle a, OS_Handle b) {
	return a.value == b.value;
}

B32 os_handle_is_zero(OS_Handle a) {
	return a.value == 0;
}

#ifdef TARGET_WINDOWS
	#include <windows.h>
#endif

void os_abort(S32 exit_code) {
#ifdef TARGET_WINDOWS
	ExitProcess(exit_code);
#else
	exit(exit_code);
#endif
}

void os_abort_with_message(S32 exit_code, String8 message) {
	os_show_message_box(OS_MessageBoxIcon_Error, str8_lit("Error Occurred!"), message);
	os_abort(exit_code);
}

#ifdef TARGET_LINUX
#include <time.h>

U64 os_now_microseconds(void) {
	struct timespec ts = {0};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	/* Seconds & Nanoseconds Conversion To Microseconds */
	U64 result = (ts.tv_sec * 1E+6) + (ts.tv_nsec / 1E+3);
	return result;
}

U64 os_now_milliseconds(void) {
	struct timespec ts = {0};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	U64 result = ts.tv_sec * 1E+3;
	return result;
}
#else
	#error No Implementation Found!
#endif

// TODO(pegvin) - Use the respective platform's provided API
void os_sleep_milliseconds(U32 msec) {
	U64 start = os_now_milliseconds();
	U64 diff = 0;
	// `diff` MIGHT be equal to `start` if os_now_milliseconds fails.
	while ((diff = start - os_now_milliseconds()) < msec && diff != start);
}
