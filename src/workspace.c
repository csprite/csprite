#include "workspace.h"

workspace_t* InitWorkspace(int WindowDims[2]) {
	workspace_t* w = (workspace_t*) malloc(sizeof(workspace_t));
	if (w == NULL) return NULL;

	w->FileHasChanged = false;
	w->ZoomLevel = 8;
	w->CanvasDims[0] = 60;
	w->CanvasDims[1] = 40;
	w->PaletteIndex = 0;
	w->ColorIndex = 0;
	w->LastColorIndex = 0;
	w->CanvasData = NULL;
	w->CurrentState = NULL;
	w->SelectedData = NULL;
	w->CanvasTex = NULL;
	w->CanvasBgTex = NULL;
	w->CanvasContRect.x = (int)(WindowDims[0] / 2) - (w->CanvasDims[0] * w->ZoomLevel / 2);
	w->CanvasContRect.y = (int)(WindowDims[1] / 2) - (w->CanvasDims[1] * w->ZoomLevel / 2);
	w->CanvasContRect.w = (int)w->CanvasDims[0] * w->ZoomLevel;
	w->CanvasContRect.h = (int)w->CanvasDims[1] * w->ZoomLevel;

	w->SelectionRect.x = 0;
	w->SelectionRect.y = 0;
	w->SelectionRect.w = 0;
	w->SelectionRect.h = 0;

	w->SelectionRectNew.x = 0;
	w->SelectionRectNew.y = 0;
	w->SelectionRectNew.w = 0;
	w->SelectionRectNew.h = 0;

	return w;
}

int FreeWorkspace(workspace_t* w) {
	if (w == NULL) {
		return -1;
	}
	if (w->CanvasTex != NULL) {
		SDL_DestroyTexture(w->CanvasTex);
		w->CanvasTex = NULL;
	}
	if (w->CanvasBgTex != NULL) {
		SDL_DestroyTexture(w->CanvasBgTex);
		w->CanvasBgTex = NULL;
	}
	if (w->SelectedData != NULL) {
		free(w->SelectedData);
		w->SelectedData = NULL;
	}
	if (w->CanvasData != NULL) {
		free(w->CanvasData);
		w->CanvasData = NULL;
	}
	if (w->CurrentState != NULL) {
		FreeHistory(&w->CurrentState);
	}

	free(w);
	return 0;
}
