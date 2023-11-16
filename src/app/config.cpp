#include <cstdio>

#include "app/config.hpp"
#include "fswrapper.hpp"
#include "log/log.h"

#include "SimpleIni.h"

namespace Fs = FileSystem;

static Cfg::Config conf;

Cfg::Config& Cfg::Get() {
	return conf;
}

void Cfg::Load() {
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(Fs::GetConfigFile().c_str()) < 0) {
		return;
	}

	conf.fps = ini.GetLongValue("program", "fps", 50);
	conf.langFileName = ini.GetValue("program", "language_file", "english.ini");
}

void Cfg::Write() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetLongValue("program", "fps", conf.fps);
	ini.SetValue("program", "language_file", conf.langFileName.c_str());
	ini.SaveFile(Fs::GetConfigFile().c_str());
}

