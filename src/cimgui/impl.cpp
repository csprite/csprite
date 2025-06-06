// Unity Build

#if defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	#pragma GCC diagnostic ignored "-Wpadded"
#elif defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma GCC diagnostic ignored "-Wpadded"
#endif

#include "cimgui/imgui/imgui.cpp"
#include "cimgui/imgui/imgui_draw.cpp"
#include "cimgui/imgui/imgui_tables.cpp"
#include "cimgui/imgui/imgui_widgets.cpp"
#include "cimgui/imgui/imgui_impl_opengl3.cpp"
#include "cimgui/imgui/imgui_impl_glfw.cpp"
#include "cimgui/cimgui.cpp"

#if defined(__GNUC__)
	#pragma GCC diagnostic pop
#elif defined(__clang__)
	#pragma clang diagnostic pop
#endif

// #include "cimgui/cimgui.cpp"
