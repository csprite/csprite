#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <glad/glad.h>
#include "../history.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LAYER_NAME_MAX 512

typedef struct {
	char       name[LAYER_NAME_MAX];
	GLuint     texture;
	uint8_t*   pixels;
	History_T* history;
} CanvasLayer_T;

int  Canvas_Init(int32_t w, int32_t h); // Initialize Canvas
void Canvas_Destroy(void); // Destroy Canvas
void Canvas_NewFrame(bool DrawCheckerboardBg); // Call Before All Of The Calls To Canvas_Layer();
void Canvas_Layer(CanvasLayer_T* c, bool UpdateTexture); // Specify What Layers To Render
void Canvas_Render(GLint ViewportPosX, GLint ViewportPosY, GLsizei ViewportWidth, GLsizei ViewportHeight); // Renders The Canvas To Screen, Call After All Of The Calls To Canvas_Layer();
void Canvas_Resize(int32_t w, int32_t h); // Resize Canvas

uint8_t* Canvas_GetRender();
GLuint   Canvas_GetFBOTex();

CanvasLayer_T* Canvas_CreateLayer();
void           Canvas_DestroyLayer(CanvasLayer_T* c);

#ifdef __cplusplus
}
#endif

