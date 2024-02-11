#include "SimpleIni.h"
#include "app/prefs.hpp"

bool Preferences::Load(const char* filePath) {
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(filePath) < 0) {
		return false;
	}

	fps = ini.GetLongValue("program", "fps", 50);
	langFileName = ini.GetValue("program", "language_file", "english.ini");

	return true;
}

void Preferences::Write(const char* filePath) {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetLongValue("program", "fps", fps);
	ini.SetValue("program", "language_file", langFileName.c_str());
	ini.SaveFile(filePath);
}

