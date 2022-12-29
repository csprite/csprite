#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <glad/glad.h>
#include "history.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LAYER_NAME_MAX 512

typedef struct {
	char                 name[LAYER_NAME_MAX];
	GLuint               texture;
	unsigned char*       pixels;
	History_T*           history;
} CanvasLayer_T;

GLuint CanvasGetFBO();
GLuint CanvasGetFBOTex();
int InitCanvas(uint32_t w, uint32_t h);
void DeInitCanvas();
void StartCanvas(bool DrawCheckerboardBg);
void DrawLayer(CanvasLayer_T* c, bool UpdateTexture);
void EndCanvas(GLint ViewportPosX, GLint ViewportPosY, GLsizei ViewportWidth, GLsizei ViewportHeight);
void ResizeCanvas(uint32_t w, uint32_t h);
uchar_t* CanvasGetRendered();

CanvasLayer_T* CreateCanvasLayer();
void DestroyCanvasLayer(CanvasLayer_T* c);

#ifdef __cplusplus
}
#endif
