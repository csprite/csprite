#include "base/memory.h"
#include "os/os.h"

#include <stdlib.h>
#include <string.h>

void* Memory_Alloc(size_t sz) {
	return malloc(sz);
}

void* Memory_AllocOrDie(size_t sz) {
	void* ptr = Memory_Alloc(sz);
	if (!ptr) {
		os_abort_with_message(1, str8_lit("Failed to allocate memory!"));
	}
	return ptr;
}

void Memory_ZeroAll(void* ptr, size_t sz) {
	memset(ptr, 0, sz);
}

void Memory_Dealloc(void* ptr) {
	free(ptr);
}
