#ifndef CSP_APP_FSWRAPPER_HPP_INCLUDED_
#define CSP_APP_FSWRAPPER_HPP_INCLUDED_
#pragma once

#include "types.h"
#include "fs/defs.hpp"

namespace FileSystem {
	String GetConfigDir();
	inline String GetConfigFile() { return FileSystem::GetConfigDir() + PATH_SEP "config.ini"; }
	inline String GetLanguagesDir() { return FileSystem::GetConfigDir() + PATH_SEP "languages"; }
	inline String GetPalettesDir() { return FileSystem::GetConfigDir() + PATH_SEP "palettes"; }
}

#endif // CSP_APP_FSWRAPPER_HPP_INCLUDED_

