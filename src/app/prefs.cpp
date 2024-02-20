#include "SimpleIni.h"
#include "app/prefs.hpp"

bool Preferences::Load(const char* filePath) {
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(filePath) < 0) {
		return false;
	}

	fps = ini.GetLongValue("program", "fps", Preferences::fps);
	fps = MIN_MAX(fps, 5, 999);
	fontSize = ini.GetLongValue("program", "font_size", Preferences::fontSize);
	fontSize = MIN_MAX(fontSize, 10, 999);
	langFileName = ini.GetValue("program", "language_file", Preferences::langFileName.c_str());

	return true;
}

void Preferences::Write(const char* filePath) {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetLongValue("program", "fps", fps);
	ini.SetLongValue("program", "font_size", fontSize);
	ini.SetValue("program", "language_file", langFileName.c_str());
	ini.SaveFile(filePath);
}
