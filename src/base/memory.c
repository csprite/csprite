#include "base/memory.h"
#include "platform/platform.h"

#include <stdlib.h>
#include <string.h>

void* Memory_Alloc(size_t sz) {
	return malloc(sz);
}

void* Memory_AllocOrDie(size_t sz) {
	void* ptr = Memory_Alloc(sz);
	if (!ptr) {
		Platform_AbortMessage(1, "Cannot Allocate More Memory");
	}
	return ptr;
}

void Memory_ZeroAll(void* ptr, size_t sz) {
	memset(ptr, 0, sz);
}

void Memory_Dealloc(void* ptr) {
	free(ptr);
}
