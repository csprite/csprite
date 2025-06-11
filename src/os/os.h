#ifndef SRC_OS_OS_H
#define SRC_OS_OS_H 1

#include "base/types.h"
#include "base/math.h"
#include "base/string.h"

typedef struct {
	U64 value;
} OS_Handle;

// Type Functions
OS_Handle os_handle_zero(void);
B32       os_handle_match(OS_Handle a, OS_Handle b);
B32       os_handle_is_zero(OS_Handle a);

// Memory
void* os_memory_reserve(U64 size);
void os_memory_unreserve(void* memory, U64 size);
void os_memory_commit(void* memory, U64 size);
void os_memory_uncommit(void* memory, U64 size);

// Abortion
void os_abort(S32 exit_code);
void os_abort_with_message(S32 exit_code, String8 message);

// Time
U64  os_now_microseconds(void); // This U64 Can Store Upto powl(2, 64) / 3.154E+15 = ~5848 Centuries.
U64  os_now_milliseconds(void); // This U64 Can Store Upto powl(2, 64) / 3.154E+12 = ~5848682 Centuries.
void os_sleep_milliseconds(U32 msec);

// Paths
Rng1DU64 os_path_basename(String8 path);
Rng1DU64 os_path_extension(String8 path);

#endif // SRC_OS_OS_H
