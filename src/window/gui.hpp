#ifndef CSP_WINDOW_GUI_HPP_INCLUDED_
#define CSP_WINDOW_GUI_HPP_INCLUDED_ 1
#pragma once

#include "window/window.hpp"

namespace Gui {
	void Init();
	void ProcessEvents(Window::Event* e);
	void Draw();
	void Destroy();

	float GetDisplayFbScaleX();
	float GetDisplayFbScaleY();
}

#endif // CSP_WINDOW_GUI_HPP_INCLUDED_
