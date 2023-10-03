#include <cstdio>

#include "app/config.hpp"
#include "fswrapper.hpp"
#include "log/log.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace Fs = FileSystem;

static Cfg::Config conf;

Cfg::Config& Cfg::Get() {
	return conf;
}

void Cfg::Load() {
	FILE* f = fopen(Fs::GetConfigFile().c_str(), "r");
	if (f == NULL) {
		log_error("fopen(...) - %s", strerror(errno));
		return;
	}

	json p = json::parse(f);
	conf.fps = p["FPS"];
	conf.langFileName = p["LangFile"];

	fclose(f);
}

void Cfg::Write() {
	FILE* f = fopen(Fs::GetConfigFile().c_str(), "w");
	if (f == NULL) {
		log_error("fopen(...) - %s", strerror(errno));
		return;
	}

	json p;
	p["FPS"] = conf.fps;
	p["LangFile"] = conf.langFileName;
	auto str = p.dump(1, '\t', false);
	if (!str.empty()) {
		fwrite(str.data(), 1, str.size(), f);
	}

	fclose(f);
}

