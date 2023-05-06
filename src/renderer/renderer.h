#pragma once

#include <SDL.h>
#include <string>

enum Renderer_API {
	R_API_DIRECT3D = 0,
	R_API_OPENGL,
	R_API_OPENGLES2,
	R_API_OPENGLES,
	R_API_METAL,
	R_API_SOFTWARE,
	R_API_COUNT,
	R_API_INVALID
};

int  R_Init(SDL_Window* window, Renderer_API api); // Initializes SDL_Renderer
void R_Destroy(void);  // Destroy ImGui & OpenGL Contexts
void R_Clear(void);    // Clear The Screen
void R_NewFrame(void); // Call Before Calling Any ImGui Functions For Rendering The Gui
void R_Present(void);  // Render Everything & Swap Buffers
SDL_Renderer* R_GetRenderer(void);

/* returns a string for a value in "Renderer_API" enum,
   compatible with SDL_HINT_RENDER_DRIVER, or returns a
   empty string for invalid argument */
std::string  R_RendererApiToString(Renderer_API api);

// returns a "Renderer_API" enum value from a corresponding string value
Renderer_API R_StringToRendererApi(std::string str);


