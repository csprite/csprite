#ifndef SRC_BASE_MEMORY_H_INCLUDED_
#define SRC_BASE_MEMORY_H_INCLUDED_ 1
#pragma once

#include <stddef.h>

// Just simple wrappers around `malloc` & `free`
void* Memory_Alloc(size_t sz);
void* Memory_AllocOrDie(size_t sz);

// Zero Everything
void Memory_ZeroAll(void* ptr, size_t sz);

void Memory_Dealloc(void* ptr);

#endif // SRC_BASE_MEMORY_H_INCLUDED_
