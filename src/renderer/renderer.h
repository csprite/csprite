#pragma once

#include <SDL2/SDL.h>

int R_Init(SDL_Window* window); // Initializes SDL_Renderer
void R_Destroy(void); // Destroy ImGui & OpenGL Contexts
void R_Clear(void); // Clear The Screen
void R_NewFrame(void); // Call Before Calling Any ImGui Functions For Rendering The Gui
void R_Present(void); // Render Everything & Swap Buffers
SDL_Renderer* R_GetRenderer();

