#ifndef CSP_APP_APP_HH_INCLUDED_
#define CSP_APP_APP_HH_INCLUDED_
#pragma once

#include "app/prefs.hpp"

bool App_Initialize(Preferences& prefs);
void App_Release(void);

#include "types.h"
#include "fs/defs.hpp"

String App_GetConfigDir();
inline String App_GetConfigFile() { return App_GetConfigDir() + PATH_SEP "config.ini"; }
inline String App_GetLanguagesDir() { return App_GetConfigDir() + PATH_SEP "languages"; }
inline String App_GetPalettesDir() { return App_GetConfigDir() + PATH_SEP "palettes"; }

#endif // !CSP_APP_APP_HH_INCLUDED_

