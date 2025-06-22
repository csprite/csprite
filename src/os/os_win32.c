#ifdef TARGET_WINDOWS

#include "os/os.h"

#include <windows.h>

NORETURN void os_abort(S32 exit_code) {
	ExitProcess(exit_code);
}

U64 os_now_microseconds(void) {
	U64 microsecond_resolution = 1;
	LARGE_INTEGER large_int_resolution;
	if (QueryPerformanceFrequency(&large_int_resolution)) {
		microsecond_resolution = large_int_resolution.QuadPart;
	}

	U64 result = 0;
	LARGE_INTEGER large_int_counter;

	if (QueryPerformanceCounter(&large_int_counter)) {
		result = (large_int_counter.QuadPart * Million(1)) / microsecond_resolution;
	}
	return result;
}

U64 os_now_milliseconds(void) {
	U64 mu = os_now_microseconds();
	return mu / 1000;
}


void* os_memory_reserve(U64 size) {
	return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

void os_memory_unreserve(void* memory, U64 size) {
	VirtualFree(memory, 0, MEM_RELEASE);
}

void os_memory_commit(void* memory, U64 size) {
	if (VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE) != 0) {
		os_abort_with_message(1, str8_lit("Failed to commit memory!"));
	}
}

void os_memory_uncommit(void* memory, U64 size) {
	VirtualFree(memory, size, MEM_DECOMMIT);
}
	
#endif // TARGET_WINDOWS
