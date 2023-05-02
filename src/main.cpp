#include "log.hpp"
#include "ini/ini.h"
#include "window/window.hpp"

int main(void) {
	Window::Init(640, 420);
	Window::ProcessEvents();
	Window::Destroy();
	return 0;
}
