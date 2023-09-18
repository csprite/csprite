#ifndef CSP_APP_CONFIG_HPP_INCLUDED_
#define CSP_APP_CONFIG_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

namespace Cfg {
	struct Config {
		u32 fps = 50;
		String langFileName = "english.json";
	};

	void Load();
	void Write();
	Cfg::Config& Get();
}

#endif // CSP_APP_CONFIG_HPP_INCLUDED_

