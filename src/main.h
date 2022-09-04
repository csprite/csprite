#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <string>

enum export_format_e { PNG, JPG };

// Little Helpers
Uint32* GetPixel(int x, int y, Uint32* data = NULL);

// Callback Function used by SDL for events
static int _EventWatcher(void* data, SDL_Event* event);

void ProcessEvents();
void UpdateCanvasRect();
void AdjustZoom(bool increase);
static void _GuiSetColors(ImGuiStyle& style);

void GenCanvasBgTex(void); // Generates a SDL_Texture and generates a checkerboard pattern for it
void GenCanvasBuff(void); // Just Allocates Memory For CanvasData & frees old memory
int UpdateTextures(void); // Updates The Textures To Use New* Canvas Dimensions
static void InitWindowIcon(void); // Basically Set's The Window Icon
static void FreeEverything(void); // Simply Free-s/Destroys Everything

void draw(int x, int y); // Draws on 1 x, y coordinate
void drawInBetween(int st_x, int st_y, int end_x, int end_y);
void fill(int x, int y, Uint32 old_color);
void drawLine(int x0, int y0, int x1, int y1);
void drawRect(int x0, int y0, int x1, int y1);
void drawCircle(int centreX, int centreY, int radius);

void SaveState();
void FreeHistory();
void Redo();
void Undo();

std::string FixFileExtension(std::string filepath);
void SaveImageFromCanvas(std::string filepath);

// Versions & Stuff

#ifndef CS_VERSION_MAJOR
	#define CS_VERSION_MAJOR 0
#endif

#ifndef CS_VERSION_MAJOR
	#define CS_VERSION_MINOR 0
#endif

#ifndef CS_VERSION_MAJOR
	#define CS_VERSION_PATCH 0
#endif

#ifndef CS_BUILD_STABLE
	#define CS_BUILD_STABLE 0
#endif

#if CS_BUILD_STABLE == 0
	#define CS_BUILD_TYPE "dev"
#else
	#define CS_BUILD_TYPE "stable"
#endif

#define VERSION_STR "v" + std::to_string(CS_VERSION_MAJOR) + \
						"." + std::to_string(CS_VERSION_MINOR) + \
						"." + std::to_string(CS_VERSION_PATCH) + \
						"-" + CS_BUILD_TYPE

#define WINDOW_TITLE_CSTR (\
		FilePath.substr(FilePath.find_last_of("/\\") + 1)\
		+ " - csprite " + VERSION_STR\
	).c_str()

#endif // end MAIN_H
