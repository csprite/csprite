#ifndef CSP_ASSETS_MANAGER_HPP_INCLUDED_
#define CSP_ASSETS_MANAGER_HPP_INCLUDED_ 1
#pragma once

namespace Assets {
	// Initializes the csprite's config directory with config & minimal theme, palette, language files
	// which can be later modified by the end-user
	bool EnsureFileSystem();
}


#endif // CSP_ASSETS_MANAGER_HPP_INCLUDED_

