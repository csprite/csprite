#include "history.h"
#include <cstring>

/*
	Function: FreeHistory()
	Takes The CurrentState Node
		- Frees All Of The Nodes Before It
		- Frees All Of The Nodes After It
*/
void FreeHistory(History_T** CurrentState) {
	if (CurrentState == NULL || (*CurrentState) == NULL) return;

	History_T* tmp;
	History_T* head = (*CurrentState)->prev;

	while (head != NULL) {
		tmp = head;
		head = head->prev;
		if (tmp != NULL && tmp->pixels != NULL) {
			delete[] tmp->pixels;
			tmp->pixels = NULL;
			free(tmp);
			tmp = NULL;
		}
	}

	head = (*CurrentState);

	while (head != NULL) {
		tmp = head;
		head = head->next;
		if (tmp != NULL && tmp->pixels != NULL) {
			delete[] tmp->pixels;
			tmp->pixels = NULL;
			free(tmp);
			tmp = NULL;
		}
	}

	(*CurrentState) = NULL;
}

/*
	Pushes Pixels On Current Canvas in "History" array at index "HistoryIndex"
	Removes The Elements in a range from "History" if "IsDirty" is true
*/
void SaveHistory(History_T** CurrentState, size_t dataSizeBytes, Pixel* data) {
	// Runs When We Did Undo And Tried To Modify The Canvas
	if (CurrentState != NULL && (*CurrentState != NULL) && (*CurrentState)->next != NULL) {
		History_T* tmp;
		History_T* head = (*CurrentState)->next; // we start freeing from the next node of current node

		while (head != NULL) {
			tmp = head;
			head = head->next;
			if (tmp->pixels != NULL) {
				delete[] tmp->pixels;
			}
			free(tmp);
		}
	}

	History_T* NewState = (History_T*) malloc(sizeof(History_T));
	NewState->pixels = new Pixel[dataSizeBytes];

	if ((*CurrentState) == NULL) {
		(*CurrentState) = NewState;
		(*CurrentState)->prev = NULL;
		(*CurrentState)->next = NULL;
	} else {
		NewState->prev = (*CurrentState);
		NewState->next = NULL;
		(*CurrentState)->next = NewState;
		(*CurrentState) = NewState;
	}

	std::memcpy((*CurrentState)->pixels, data, dataSizeBytes * sizeof(Pixel));
}

