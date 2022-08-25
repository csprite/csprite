#ifndef MOUSE_H
#define MOUSE_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define _TEX_ARRAY_SIZE 3

typedef enum mouse_t {
	DEFAULT, CLOSE_HAND, EYEDROPPER
} mouse_t;

class Mouse {
private:
	mouse_t CurrentCursor = DEFAULT;
	SDL_Texture* tex[_TEX_ARRAY_SIZE] = { NULL };
	SDL_Rect rect;
	SDL_Rect point;
	SDL_Texture* loadPngToTex(SDL_Renderer* ren, const char* cursorImgPath);

public:
	Mouse(SDL_Renderer* ren);
	~Mouse();
	void Update();
	void Draw(SDL_Renderer* ren);
	void SetCursor(mouse_t cursorType);
};

#endif