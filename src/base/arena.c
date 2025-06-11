#include "base/arena.h"
#include "base/types.h"
#include "os/os.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#define DEFAULT_ALIGNMENT sizeof(void*)
#define ARENA_MAX_RESERVE GB(16)
#define ARENA_COMMIT_SIZE KB(64)

Arena arena_init(void) {
	Arena arena = {0};
	arena.max_size = ARENA_MAX_RESERVE;
	arena.memory = os_memory_reserve(arena.max_size);
	arena.alloc_position = 0;
	arena.commit_position = 0;

	if (Unlikely(arena.memory == NULL)) {
		os_abort_with_message(1, str8_lit("Unexpected memory allocation failure!"));
	}

	return arena;
}

void arena_clear(Arena* arena) {
	arena_dealloc(arena, arena->alloc_position);
}

void arena_release(Arena* arena) {
	os_memory_unreserve(arena->memory, arena->max_size);
}

void* arena_alloc(Arena* arena, U64 size) {
	size = AlignUpPow2(size, DEFAULT_ALIGNMENT);

	if (arena->alloc_position + size > arena->commit_position) {
		U64 commit_size = size;

		commit_size += ARENA_COMMIT_SIZE - 1;
		commit_size -= commit_size % ARENA_COMMIT_SIZE;

		if (Unlikely(arena->commit_position >= arena->max_size)) {
			os_abort_with_message(1, str8_lit("Dynamic-Size Arena is out of memory!"));
		} else {
			os_memory_commit(arena->memory + arena->commit_position, commit_size);
			arena->commit_position += commit_size;
		}
	}

	void* memory = arena->memory + arena->alloc_position;
	arena->alloc_position += size;
	return memory;
}

void* arena_alloc_zero(Arena* arena, U64 size) {
	void* result = arena_alloc(arena, size);
	memset(result, 0, size);
	return result;
}

void arena_dealloc(Arena* arena, U64 size) {
	if (size > arena->alloc_position) {
		size = arena->alloc_position;
	}
	arena->alloc_position -= size;
}

void arena_dealloc_to(Arena* arena, U64 pos) {
	if (pos > arena->max_size) {
		pos = arena->max_size;
	}
	arena->alloc_position = pos;
}

void* arena_alloc_from_buffer(Arena* arena, void* ptr, U64 size) {
	void* raised = arena_alloc(arena, size);
	memcpy(raised, ptr, size);
	return raised;
}

void* arena_alloc_array_sized(Arena* arena, U64 elem_size, U64 count) {
	return arena_alloc(arena, elem_size * count);
}

ArenaTemp arena_begin_temp(Arena* arena) {
	ArenaTemp temp = {
		.arena = arena,
		.pos = arena->alloc_position
	};
	return temp;
}

void arena_end_temp(ArenaTemp temp) {
	arena_dealloc_to(temp.arena, temp.pos);
}
