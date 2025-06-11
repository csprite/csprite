#ifndef SRC_BASE_ARENA_H
#define SRC_BASE_ARENA_H 1

#include <stdlib.h>
#include "base/types.h"

typedef struct {
	U8* memory;
	U64 max_size;
	U64 alloc_position;
	U64 commit_position;
} Arena;

typedef struct {
	Arena* arena;
	U64 pos;
} ArenaTemp;

Arena arena_init(void);
void arena_clear(Arena* arena);
void arena_release(Arena* arena);

void* arena_alloc(Arena* arena, U64 size);
void* arena_alloc_zero(Arena* arena, U64 size);
void* arena_alloc_from_buffer(Arena* arena, void* ptr, U64 size);
void* arena_alloc_array_sized(Arena* arena, U64 elem_size, U64 count);
void  arena_dealloc(Arena* arena, U64 size);
void  arena_dealloc_to(Arena* arena, U64 pos);

ArenaTemp arena_begin_temp(Arena* arena);
void      arena_end_temp(ArenaTemp temp);

#endif // SRC_BASE_ARENA_H
