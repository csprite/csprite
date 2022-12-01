#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uchar_t;

typedef struct history {
	uchar_t*         pixels;
	struct history*  next;
	struct history*  prev;
} History_T;

/*
	Function: FreeHistory
	Description: Free's all of the memory allocated by the buffer & set's it to NULL
	Remarks:
		- It's safe to pass NULL as argument
		- It's not recommended to pass NULL or invalid pointers
*/
void FreeHistory(History_T** CurrentState);

/*
	Function: SaveHistory
	Description: Save's the given data to the buffer
	Remarks:
		- It's safe to pass the un-initialized buffer, it allocated the memory for it
*/
void SaveHistory(History_T** CurrentState, size_t dataSizeBytes, uchar_t* data);

// HISTORY_UNDO(History_T* state, unsigned int size, uchar_t* data)
#define HISTORY_UNDO(state, size, data)         \
	if (state->prev != NULL) {                  \
		state = state->prev;                    \
		memcpy(data, state->pixels, size);      \
	}

// HISTORY_REDO(History_T* state, unsigned int size, uchar_t* data)
#define HISTORY_REDO(state, size, data)        \
	if (state->next != NULL) {                 \
		state = state->next;                   \
		memcpy(data, state->pixels, size);     \
	}

#ifdef __cplusplus
}
#endif

