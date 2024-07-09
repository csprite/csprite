#include <stdio.h>

#include "app/window.h"
#include "cimgui.h"

int main(void) {
	if (WindowCreate("csprite", 320, 240, 1)) {
		return 1;
	}

	WindowSetBG(0, 0, 0);
	WindowSetMaxFPS(60);

	while (!WindowShouldClose()) {
		WindowNewFrame();
		if (igBegin("Main", NULL, 0)) {
			igText("Halo World!");
			igEnd();
		}
		WindowEndFrame();
	}

	WindowDestroy();
	return 0;
}
