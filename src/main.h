#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <string>
#include "GLFW/glfw3.h"

enum export_format_e { PNG, JPG };

// Callbacks
void ProcessInput(GLFWwindow* window);
void ScrollCallback(GLFWwindow* window, double, double);
void KeyCallback(GLFWwindow* window, int, int, int, int);
void FrameBufferSizeCallback(GLFWwindow* window, int, int);
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// Canvas Zoom
void ViewportSet();
void ZoomNLevelViewport(); // Zooms And Levels The Canvas.
void AdjustZoom(bool increase);

// Little Helpers
unsigned char * GetPixel(int x, int y);

// Canvas Operations
void fill(int x, int y, unsigned char *old_colour);
void draw(int x, int y); // Draws on 1 x, y coordinate
void drawInBetween(int st_x, int st_y, int end_x, int end_y); // Draws Between The Given 2 x, y coordinates
void drawLine(int lStartX, int lStartY, int lEndX, int lEndY);
void drawRect(int x0, int y0, int x1, int y1);

// File IO
std::string FixFileExtension(std::string filepath);
void SaveImageFromCanvas(std::string filepath);

// Undo/Redo Related
int Undo();
int Redo();
void SaveState();
void FreeHistory();

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
