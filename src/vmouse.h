#ifndef V_MOUSE_H
#define V_MOUSE_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _VMOUSE_TEX_ARRAY_SIZE 3

// These icon types directly correspond the the index in the texture array
typedef enum vmouse_icon_t {
	DEFAULT = 0,
	CLOSE_HAND = 1,
	EYEDROPPER = 2
} vmouse_icon_t;

/*
	Function: VirtualMouseInit
	Description: Allocate Resources & Load Virtual Mouse's Icons
	Remarks:
		- It is safe to call this function twice, it releases the memory using VirtualMouseFree
		- Not recommended to call it twice tho.
*/
int VirtualMouseInit(SDL_Renderer* ren);

/*
	Function: VirtualMouseFree
	Description: Free The Resources VirtualMouseInit Allocates Internally
	Remarks:
		- It is safe to call this function twice, it checks stuff before freeing them.
		- Not recommended to call it twice tho.
*/
int VirtualMouseFree();

/*
	Function: VirtualMouseSet
	Description: Set The Icon Of Virtual Mouse
	Remarks:
		- It is safe to call this function before VirtualMouseInit call
		- It is safe to call this function after VirtualMouseFree call
*/
void VirtualMouseSet(vmouse_icon_t iconType);

/*
	Function: VirtualMouseUpdate
	Description: Update Position of The Virtual Mouse
	Remarks:
		- Uses SDL_GetMouseState() to get the 'real' mouse cursor's position
*/
void VirtualMouseUpdate();

/*
	Function: VirtualMouseDraw
	Description: Draw The Mouse Using SDL Renderer
*/
void VirtualMouseDraw(SDL_Renderer* ren);

#ifdef __cplusplus
}
#endif

#endif // V_MOUSE_H
