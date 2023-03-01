#include "palette.h"
#include "system.h"
#include "log/log.h"
#include "assets.h"

// Forward Declarations
static int OnSysDirList(const char *dir, const char *fname, void* data);
static int OnAssetMgrList(int i, const char *fname);

void Palette::AddColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	colors.push_back({ r, g, b, a });
}

void Palette::RemoveColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	for (int32_t i = 0; i < colors.size(); ++i) {
		Color_T color = colors[i];
		if (color.r == r && color.g == g && color.b == b && color.a == a) {
			colors.erase(colors.begin() + i);
		}
	}
}

PaletteManager::PaletteManager() {
	presets = Palette_LoadAll();
	palette.AddColor(0, 0, 0);
	palette.AddColor(255, 255, 255);
	SelectedColorIdx = 0;
	PrimaryColor = palette.colors[SelectedColorIdx];
}

PaletteManager::~PaletteManager() {
	Palette_ReleaseAll(presets);
}

void PaletteManager::SetPreset(Palette& p) {
	palette = p;
}

void PaletteManager::SetSelectedColorIdx(int32_t idx) {
	SelectedColorIdx = idx;
	PrimaryColor = palette.colors[idx];
}

std::vector<Palette>* Palette_LoadAll() {
	char* pallete_dir_path = Palettes_GetDir();
	char dir[SYS_PATHNAME_MAX] = "";
	strncpy(dir, pallete_dir_path, SYS_PATHNAME_MAX);

	// Check If Palettes Directory On Disk Is Empty, If So Extract The Built-in Palettes
	int numOfPalettes = Sys_ListDirContents((const char*)dir, NULL, NULL);
	if (numOfPalettes <= 0) {
		Sys_MakeDirRecursive(dir);
		Assets_List("data/palettes/", OnAssetMgrList);
	}

	numOfPalettes = Sys_ListDirContents((const char*)dir, NULL, NULL);
	if (numOfPalettes <= 0) {
		log_error("cannot extract the palettes!\n");
		return NULL;
	}

	auto palettes = new std::vector<Palette>;
	Sys_ListDirContents((const char*)dir, OnSysDirList, palettes);

	return palettes;
}

void Palette_ReleaseAll(std::vector<Palette>* palettes) {
	delete palettes;
}

Palette* Palette_LoadCsv(const char* csvText) {
	Palette* p = new Palette;

	char* text = strdup(csvText);
	char* token = NULL;

	// First We Find Out Total Elements, To Allocate Array
	token = strtok(text, ",");
	p->name = std::string(token);
	token = strtok(NULL, ",");
	p->author = std::string(token);

	token = strtok(NULL, ","); // First Color
	// XX - Todo Fix When Author Name Or Palette Name is Not Present it glitches
	while (token != NULL) {
		uint32_t r = 0, g = 0, b = 0;
		sscanf(token, "%02x%02x%02x", &r, &g, &b);
		p->colors.push_back({
			static_cast<uint8_t>(r),
			static_cast<uint8_t>(g),
			static_cast<uint8_t>(b),
			255
		});
		token = strtok(NULL, ",");
	}

	free(text);
	if (token != NULL) free(token);
	return p;
}

char* Palettes_GetDir() {
	char* configdir = Sys_GetConfigDir();
	static char configPath[SYS_PATHNAME_MAX] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_warn("cannot get the config directory!\n");
			snprintf(configPath, SYS_PATHNAME_MAX, "palettes");
			Sys_MakeDirRecursive(configPath);
		} else {
			snprintf(configPath, SYS_PATHNAME_MAX, "%s%ccsprite%cpalettes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			Sys_MakeDirRecursive(configPath);
		}
	}

	return configPath;
}

static int OnSysDirList(const char *dir, const char *fname, void* data) {
	std::vector<Palette>* palettes = static_cast<std::vector<Palette>*>(data);

	char fullPath[SYS_PATHNAME_MAX] = "";
	snprintf(fullPath, SYS_PATHNAME_MAX, "%s/%s", dir, fname);

	FILE* fp = fopen(fullPath, "r");
	size_t size = Sys_GetFileSize(fp); // XX - Do Error (-1) Checking
	char* csvtxt = (char*) malloc((size + 1) * sizeof(char));
	memset(csvtxt, '\0', size + 1);
	fread(csvtxt, size + 1, 1, fp);
	Palette* p = Palette_LoadCsv(csvtxt);
	palettes->push_back(*p);
	delete p;

	free(csvtxt);
	fclose(fp);
	return 0;
}

static int OnAssetMgrList(int i, const char *fname) {
	FILE* file = NULL;
	const char* data = NULL;
	char* pallete_dir_path = Palettes_GetDir();
	char dir[SYS_PATHNAME_MAX] = "";
	char* fileName = Sys_GetBasename(fname);
	snprintf(dir, SYS_PATHNAME_MAX, "%s/%s", pallete_dir_path, fileName);

	free(fileName);
	fileName = NULL;

	data = (const char*)Assets_Get(fname, NULL);
	file = fopen(dir, "wb");
	if (file) {
		fprintf(file, "%s", data);
		fclose(file);
	} else {
		log_error("cannot open file to write\n");
		return -1;
	}
	return 0;
}

