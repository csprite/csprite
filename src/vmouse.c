#include "vmouse.h"
#include "assets.h"
#include "log/log.h"

vmouse_icon_t CurrentCursor = DEFAULT;
SDL_Texture* tex[_VMOUSE_TEX_ARRAY_SIZE] = { NULL };
SDL_Rect rect;
SDL_Rect point;

// These file names directly correspond the the index in the texture array & icon type in vmouse_icon_t enum
const char* _cursor_img_path[_VMOUSE_TEX_ARRAY_SIZE] = {
	"data/cursors/default.png",
	"data/cursors/closehand.png",
	"data/cursors/eyedropper.png"
};

static SDL_Texture* _loadPngToTex(SDL_Renderer* ren, const char* cursorImgPath) {
	SDL_Texture* _tex = NULL;
	unsigned char* cursorImg = (unsigned char*)assets_get(cursorImgPath, NULL);
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		cursorImg,
		32, 32, 32, 32 * 4,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	_tex = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	return _tex;
}

int VirtualMouseInit(SDL_Renderer* ren) {
	for (int i = 0; i < _VMOUSE_TEX_ARRAY_SIZE; ++i) {
		if (tex[i] != NULL) {
			VirtualMouseFree();
			break;
		}
	}

	for (int i = 0; i < _VMOUSE_TEX_ARRAY_SIZE; ++i) {
		tex[i] = _loadPngToTex(ren, _cursor_img_path[i]);
	}

	rect.w = 32;
	rect.h = 32;
	point.w = 1;
	point.h = 1;
	if (SDL_ShowCursor(SDL_DISABLE) < 0) {
		log_error("failed to hide the cursor: %s", SDL_GetError());
		return -1;
	}
	return 0;
}

int VirtualMouseFree() {
	for (int i = 0; i < _VMOUSE_TEX_ARRAY_SIZE; ++i) {
		if (tex[i] != NULL) {
			SDL_DestroyTexture(tex[i]);
			tex[i] = NULL;
		}
	}

	if (SDL_ShowCursor(SDL_ENABLE) < 0) {
		log_error("failed to show the cursor: %s", SDL_GetError());
		return -1;
	}

	return 0;
}

void VirtualMouseSet(vmouse_icon_t iconType) {
	CurrentCursor = iconType;
}

void VirtualMouseUpdate() {
	SDL_GetMouseState(&rect.x, &rect.y);
	point.x = rect.x;
	point.y = rect.y;
}

void VirtualMouseDraw(SDL_Renderer* ren) {
	SDL_RenderCopy(ren, tex[CurrentCursor], NULL, &rect);
}
