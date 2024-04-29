#include <cerrno>
#include <cstring>

#include "app/app.hh"
#include "log/log.h"

#include "app/misc.hpp"

#include "fs/fs.hpp"

#include "palette/palette.hpp"
#include "app/i18n/strings.hpp"

#include "imbase/window.hpp"

#include "assets/assets.h"

bool App_Initialize(Preferences& prefs) {
	/* Enable VT100 Mode For Logging */
	EnableVT100();

	/* Ensure Required Directories Exist */
	if (!FileSystem::MakeDirRecursive(App_GetConfigDir())) {
		log_error("FileSystem::MakeDirRecursive(...) - %s", strerror(errno));
		return false;
	}

	if (!FileSystem::MakeDir(App_GetLanguagesDir())) {
		log_error("FileSystem::MakeDir(...) - %s", strerror(errno));
		return false;
	}
	if (!FileSystem::MakeDir(App_GetPalettesDir())) {
		log_error("FileSystem::MakeDir(...) - %s", strerror(errno));
		return false;
	}

	/* Load User Preferences */
	if (!prefs.Load(App_GetConfigFile().c_str())) {
		prefs = Preferences();
	}

	/* Update Palette & Language File Entries */
	PaletteHelper::UpdateEntries();
	UIString::UpdateEntries();

	/* Try To Load Language or Load Default */
	if (prefs.langFileName.empty() || !UIString::LoadFile(prefs.langFileName)) {
		UIString::LoadDefault();
	}

	/* Initialize a new Window & Set FPS */
	if (ImBase::Window::Init(700, 500, "csprite") != 0) {
		return false;
	}
	ImBase::Window::SetMaxFPS(prefs.fps);

	/* Load UI Font */
	const ImGuiIO& io = ImGui::GetIO();
	int fontDataSize = 0;
	ImVector<ImWchar> FontRanges;
	ImFontGlyphRangesBuilder FontBuilder;
	FontBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	FontBuilder.AddRanges(UIString::GetRanges());
	FontBuilder.BuildRanges(&FontRanges);
	io.Fonts->AddFontFromMemoryCompressedTTF(
		assets_get("data/fonts/NotoSansMono.ttf", &fontDataSize),
		fontDataSize, prefs.fontSize, nullptr, FontRanges.Data
	);
	io.Fonts->Build();
	if (!io.Fonts->IsBuilt()) {
		log_error("io.Fonts->Build() - failed to build the font atlas");
		return false;
	}

	return true;
}

void App_Release(void) {
	ImBase::Window::Destroy();
}

#include <cstdlib> // getenv

#if defined(TARGET_LINUX)
	#include <unistd.h>
	#include <pwd.h>
#endif

String App_GetConfigDir() {
	String fullPath = "";

#if defined(TARGET_LINUX)
	const char* configHome = NULL;
	configHome = getenv("XDG_CONFIG_HOME");
	if (!configHome) {
		configHome = getenv("HOME");
		if (!configHome) {
			configHome = getpwuid(getuid())->pw_dir;
			if (!configHome) return NULL;
		}
		fullPath = String(configHome) + PATH_SEP ".config" PATH_SEP "csprite";
	} else {
		fullPath = String(configHome) + PATH_SEP "csprite";
	}
#elif defined(TARGET_WINDOWS)
	const char* appdata = getenv("APPDATA");
	fullPath = String(appdata) + PATH_SEP "csprite";
#elif defined(TARGET_APPLE)
	const char* home = getenv("HOME");
	fullPath = String(home) + PATH_SEP "Library/Application Support";
#else
	#error "No Implementation of Sys::GetConfigDir, please report this issue!"
#endif

	return fullPath;
}

