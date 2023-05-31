#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "types.hpp"
#include "pixel/pixel.hpp"

enum export_format_e { PNG, JPG };

// Canvas Zoom
void ViewportSet();
void ZoomNLevelViewport(); // Zooms And Levels The Canvas.
void AdjustZoom(bool increase);

// Little Helpers
Pixel& GetPixel(int x, int y);

// File IO
std::string FixFileExtension(std::string filepath);
void SaveImageFromCanvas(std::string filepath);

// Undo/Redo Related
int Undo();
int Redo();
void SaveState();
void FreeHistory();

#endif // end MAIN_H
