#ifndef HISTORY_H
#define HISTORY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CANVAS_DATA_CHANGE
} command_t;

typedef unsigned int Uint32;

typedef struct history {
	Uint32* pixels;
	struct history* next;
	struct history* prev;
} history_t;

/*
	Function: FreeHistory
	Description: Free's all of the memory allocated by the buffer & set's it to NULL
	Remarks:
		- It's safe to pass NULL as argument
		- It's not recommended to pass NULL or invalid pointers
*/
void FreeHistory(history_t** CurrentState);

/*
	Function: SaveHistory
	Description: Save's the given data to the buffer
	Remarks:
		- It's safe to pass the un-initialized buffer, it allocated the memory for it
*/
void SaveHistory(history_t** CurrentState, Uint32 dataSizeBytes, Uint32* data);

// HISTORY_UNDO(history_t* state, unsigned int size, Uint32* data)
#define HISTORY_UNDO(state, size, data)         \
	if (state->prev != NULL) {                  \
		state = state->prev;                    \
		memcpy(data, state->pixels, size);      \
	}

// HISTORY_REDO(history_t* state, unsigned int size, Uint32* data)
#define HISTORY_REDO(state, size, data)        \
	if (state->next != NULL) {                 \
		state = state->next;                   \
		memcpy(data, state->pixels, size);     \
	}


#ifdef __cplusplus
}
#endif

#endif // HISTORY_H