#include "log.hpp"
#include "ini/ini.h"
#include "window/window.hpp"
#include <SDL_timer.h>

int main(void) {
	AppWindow_Init(640, 420);

	SDL_Delay(5000);

	AppWindow_Destroy();
	return 0;
}
