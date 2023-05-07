#ifndef CSP_SRC_CONFIG_HPP_INCLUDED_
#define CSP_SRC_CONFIG_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"
#include "renderer/renderer.h"

struct AppConfig {
	u16    FramesPerSecond;
	String ThemeName;
	Renderer_API RenderDriver;
	Pixel  CheckerboardColor1;
	Pixel  CheckerboardColor2;

	AppConfig& operator = (const AppConfig& rhs);
	bool operator == (const AppConfig& rhs) const;
	bool operator != (const AppConfig& rhs) const;
};

AppConfig* LoadConfig(void);
int WriteConfig(AppConfig* s);

#endif // CSP_SRC_CONFIG_HPP_INCLUDED_
