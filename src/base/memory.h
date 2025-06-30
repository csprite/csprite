#ifndef SRC_BASE_MEMORY_H
#define SRC_BASE_MEMORY_H 1
#pragma once

#ifdef BUILD_HAS_ASAN
	#include <sanitizer/asan_interface.h>

	#define memory_asan_poison(ptr, sz)   __asan_poison_memory_region((ptr), (sz))
	#define memory_asan_unpoison(ptr, sz) __asan_unpoison_memory_region((ptr), (sz))
#else
	#define memory_asan_poison(ptr, sz)   ((void)(ptr), (void)(sz))
	#define memory_asan_unpoison(ptr, sz) ((void)(ptr), (void)(sz))
#endif

#endif // SRC_BASE_MEMORY_H
