#ifndef CSPRITE_SRC_APP_IMGUI_H_INCLUDED_
#define CSPRITE_SRC_APP_IMGUI_H_INCLUDED_
#pragma once

// Wrapper For cimgui.h To disable warnings

#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__) // Because clang defines __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	#pragma GCC diagnostic ignored "-Wpadded"
	#pragma GCC diagnostic ignored "-Wtypedef-redefinition"
#elif defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-Wpadded"
	#pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif

#include "cimgui/cimgui.h"

#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
	#pragma GCC diagnostic pop
#elif defined(__clang__)
	#pragma clang diagnostic pop
#endif

#endif // CSPRITE_SRC_APP_IMGUI_H_INCLUDED_
