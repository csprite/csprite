#ifndef CSPRITE_SRC_APP_IMGUI_H_INCLUDED_
#define CSPRITE_SRC_APP_IMGUI_H_INCLUDED_
#pragma once

// Wrapper For cimgui.h To disable warnings

#if defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	#pragma GCC diagnostic ignored "-Wpadded"
#elif defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma GCC diagnostic ignored "-Wpadded"
#endif

#include "cimgui/cimgui.h"

#if defined(__GNUC__)
	#pragma GCC diagnostic pop
#elif defined(__clang__)
	#pragma clang diagnostic pop
#endif

#endif // CSPRITE_SRC_APP_IMGUI_H_INCLUDED_
