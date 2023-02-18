#ifndef _MAIN_H
#define _MAIN_H 1

#include "ifileio/ifileio.h"
#include "config.h"

void SetCanvasDims(int32_t w, int32_t h);
void GetCanvasDims(int32_t* w, int32_t* h);
int OpenNewFile(const char* fileName);

Config_T* GetAppConfig();
CanvasLayerArr_T* GetCanvasLayers();
const char* GetFilePath();
void SetFilePath(const char* filePath);
void SetFrameRate(int32_t fps);

void Canvas_Undo();
void Canvas_Redo();
bool CanUndo();
bool CanRedo();

#endif // _MAIN_H


