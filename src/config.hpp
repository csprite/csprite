#ifndef CSP_SRC_CONFIG_HPP_INCLUDED_
#define CSP_SRC_CONFIG_HPP_INCLUDED_ 1
#pragma once

#include <string>
#include "renderer/renderer.h"

struct Config_T {
	uint16_t     Max_FPS;
	std::string  Theme_Name;
	Renderer_API RenderDriver;
	uint8_t      CheckerboardColor1[3];
	uint8_t      CheckerboardColor2[3];

	Config_T& operator = (const Config_T& rhs);
};

Config_T* LoadConfig(void);
int WriteConfig(Config_T* s);

#endif // CSP_SRC_CONFIG_HPP_INCLUDED_
