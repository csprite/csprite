#ifndef SDL2_WRAPPER_H
#define SDL2_WRAPPER_H

// Simple Wrapper Around SDL2 Includes

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#endif
