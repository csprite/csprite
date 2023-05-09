#ifndef CSP_HISTORY_H_INCLUDED_
#define CSP_HISTORY_H_INCLUDED_ 1
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "pixel/pixel.hpp"

typedef struct history {
	Pixel*          pixels;
	struct history* next;
	struct history* prev;
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
void SaveHistory(History_T** CurrentState, size_t dataSizeBytes, Pixel* data);

// HISTORY_UNDO(History_T* state, unsigned int size, Pixel* data)
#define HISTORY_UNDO(state, size, data)         \
	if (state->prev != NULL) {                  \
		state = state->prev;                    \
		std::memcpy(data, state->pixels, size); \
	}

// HISTORY_REDO(History_T* state, unsigned int size, Pixel* data)
#define HISTORY_REDO(state, size, data)         \
	if (state->next != NULL) {                  \
		state = state->next;                    \
		std::memcpy(data, state->pixels, size); \
	}

#endif // CSP_HISTORY_H_INCLUDED_
