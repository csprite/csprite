#ifndef CSP_WINDOW_GUI_HPP_INCLUDED_
#define CSP_WINDOW_GUI_HPP_INCLUDED_ 1

#include "window/window.hpp"

namespace Gui {
	void Init();
	void ProcessEvents(Window::Event* e);
	void BuildCmdBuffer();
	void Destroy();

	float GetDisplayFbScaleX();
	float GetDisplayFbScaleY();
}

#endif // CSP_WINDOW_GUI_HPP_INCLUDED_
