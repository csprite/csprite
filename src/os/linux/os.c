#ifdef TARGET_LINUX

#include "os/os.h"

#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>

NORETURN void os_abort(S32 exit_code) {
	exit(exit_code);
}

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

void* os_memory_reserve(U64 size) {
	return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

void os_memory_unreserve(void* memory, U64 size) {
	munmap(memory, size);
}

void os_memory_commit(void* memory, U64 size) {
	mprotect(memory, size, PROT_READ | PROT_WRITE);
}

void os_memory_uncommit(void* memory, U64 size) {
	mprotect(memory, size, PROT_NONE);
}

#endif // TARGET_LINUX
