#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "types.hpp"
#include "pixel/pixel.hpp"

enum export_format_e { PNG, JPG };

// Canvas Zoom
void ZoomNCenterVP(); // Zooms And Centers The Viewport.
void AdjustZoom(bool increase);

// File IO
std::string FixFileExtension(std::string filepath);
void SaveImageFromCanvas(std::string filepath);

// Undo/Redo Related
int Undo();
int Redo();
void SaveState();
void FreeHistory();

#endif // end MAIN_H
