#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "history.h"
#include "palette.h"
#include "sdl2_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	bool FileHasChanged;
	int ZoomLevel;
	int CanvasDims[2];
	unsigned int PaletteIndex;
	unsigned int ColorIndex;
	unsigned int LastColorIndex;
	char* FilePath;
	Uint32* CanvasData;   // Holds canvas data being displayed on screen
	Uint32* SelectedData; // Holds canvas data which is selected
	SDL_Texture* CanvasTex;
	SDL_Texture* CanvasBgTex;
	SDL_Rect CanvasContRect;   // Rectangle In Which Our Canvas Will Be Placed
	SDL_Rect SelectionRect;    // Rectangle Which Represents Our Selection
	SDL_Rect SelectionRectNew; // Rectangle Which Represents Our Selection But Moved To new Place
	history_t* CurrentState;   // Undo-Redo State
} workspace_t;

workspace_t* InitWorkspace(int WindowDims[2]);
int FreeWorkspace(workspace_t* w);

#ifdef __cplusplus
}
#endif

#endif
