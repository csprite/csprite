#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H 1

#include <string>
#include "renderer/renderer.h"

struct Config_T {
	uint16_t     Max_FPS;
	std::string  Theme_Name;
	Renderer_API RenderDriver;
	uint8_t      CheckerboardColor1[3];
	uint8_t      CheckerboardColor2[3];

	Config_T& operator = (const Config_T& a) {
		this->Max_FPS = a.Max_FPS;
		this->Theme_Name = a.Theme_Name;
		this->RenderDriver = a.RenderDriver;
		this->CheckerboardColor1[0] = a.CheckerboardColor1[0];
		this->CheckerboardColor1[1] = a.CheckerboardColor1[1];
		this->CheckerboardColor1[2] = a.CheckerboardColor1[2];

		this->CheckerboardColor2[0] = a.CheckerboardColor2[0];
		this->CheckerboardColor2[1] = a.CheckerboardColor2[1];
		this->CheckerboardColor2[2] = a.CheckerboardColor2[2];
		return *this;
	}
};

Config_T* LoadConfig(void);
int WriteConfig(Config_T* s);

#endif // _APP_CONFIG_H


