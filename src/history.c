#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"

/*
	Function: FreeHistory()
	Takes The CurrentState Node
		- Frees All Of The Nodes Before It
		- Frees All Of The Nodes After It
*/
void FreeHistory(cvstate_t** CurrentState) {
	if (CurrentState == NULL || (*CurrentState) == NULL) return;

	cvstate_t* tmp;
	cvstate_t* head = (*CurrentState)->prev;

	while (head != NULL) {
		tmp = head;
		head = head->prev;
		if (tmp != NULL && tmp->pixels != NULL) {
			free(tmp->pixels);
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
			free(tmp->pixels);
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
void SaveHistory(cvstate_t** CurrentState, Uint32 dataSizeBytes, Uint32* data) {
	// Runs When We Did Undo And Tried To Modify The Canvas
	if (CurrentState != NULL && (*CurrentState != NULL) && (*CurrentState)->next != NULL) {
		cvstate_t* tmp;
		cvstate_t* head = (*CurrentState)->next; // we start freeing from the next node of current node

		while (head != NULL) {
			tmp = head;
			head = head->next;
			if (tmp->pixels != NULL) {
				free(tmp->pixels);
			}
			free(tmp);
		}
	}

	cvstate_t* NewState = (cvstate_t*) malloc(sizeof(cvstate_t));
	NewState->pixels = (Uint32*) malloc(dataSizeBytes);

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

	memset((*CurrentState)->pixels, 0, dataSizeBytes);
	memcpy((*CurrentState)->pixels, data, dataSizeBytes);
}

