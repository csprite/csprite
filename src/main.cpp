#include "log.hpp"
#include "ini/ini.h"
#include "window/window.hpp"
#include <SDL_timer.h>

int main(void) {
	WindowNS::Init(640, 420);
	WindowNS::ProcessEvents();
	WindowNS::Destroy();
	return 0;
}
