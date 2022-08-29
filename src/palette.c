#define _DEFAULT_SOURCE

#include <string.h>
#include <sys/stat.h>

#include "cconfig.h"
#include "palette.h"
#include "assets.h"
#include "system.h"
#include "log/log.h"

// Forward Declarations
static char* filenameFromPath(const char* path);
static char* getPaletteDirPath();
static int OnSysDirList(const char *dir, const char *fname, void* data);
static int OnAssetMgrList(int i, const char *fname);

int FreePaletteArr(palette_arr_t* pArr) {
	if (pArr == NULL) {
		log_error("Palette Array Pointer is NULL!");
		return -1;
	}

	if (pArr->entries != NULL) {
		for (int i = 0; i < pArr->numOfEntries; ++i) {
			FreePalette(pArr->entries[i]);
			pArr->entries[i] = NULL;
		}
		free(pArr->entries);
		pArr->entries = NULL;
	}

	free(pArr);
	return 0;
}


int FreePalette(palette_t* palette) {
	if (palette == NULL) {
		log_error("Palette Pointer is NULL!");
		return -1;
	}

	if (palette->entries == NULL) {
		log_error("Palette Entries are NULL!");
		return -1;
	}

	free(palette->entries);
	free(palette);
	return 0;
}

palette_t* LoadCsvPalette(const char* csvText) {
	palette_t* newPalette = malloc(sizeof(palette_t));
	if (newPalette == NULL) {
		log_error("'malloc' returned NULL, cannot allocate memory for the palette!");
		return NULL;
	}

	char* text = strdup(csvText);
	char* token = NULL;
	int totalSize = 0;

	// First We Find Out Total Elements, To Allocate Array
	token = strtok(text, ",");
	while(token != NULL) {
		token = strtok(NULL, ",");
		++totalSize;
	}

	free(text);
	text = NULL;
	text = strdup(csvText);

	newPalette->numOfEntries = totalSize - 2; // Number Of Colors = Total Size - 2 (Palette Name, Author)
	newPalette->entries = malloc(sizeof(palette_entry_t) * newPalette->numOfEntries);

	// First We Find Out Total Elements, To Allocate Array
	token = strtok(text, ","); // Palette Name
	strcpy(newPalette->name, token);
	token = strtok(NULL, ","); // Author Name
	token = strtok(NULL, ","); // First Color

	// XX - Todo Fix When Author Name Or Palette Name is Not Present it glitches

	for (int i = 0; token != NULL; ++i) {
		unsigned int r = 0, g = 0, b = 0;
		sscanf(token, "%02x%02x%02x", &r, &g, &b);
		newPalette->entries[i] = RGBA2UINT32(r, g, b, 255);
		token = strtok(NULL, ",");
	}

	free(text);
	text = NULL;

	if (token != NULL) {
		free(token);
		token = NULL;
	}

	return newPalette;
}

palette_arr_t* PalletteLoadAll() {
	char* pallete_dir_path = getPaletteDirPath();
	char dir[CC_PATH_SIZE_MAX + 128] = "";
	strncpy(dir, pallete_dir_path, CC_PATH_SIZE_MAX);

	int numOfPalettes = sys_list_dir((const char*)dir, NULL, NULL);
	if (numOfPalettes <= 0) {
		sys_make_dir(dir);
		assets_list("data/palettes/", OnAssetMgrList);
	}

	numOfPalettes = sys_list_dir((const char*)dir, NULL, NULL);
	if (numOfPalettes <= 0) {
		log_error("cannot extract the palettes!");
		return NULL;
	}

	palette_arr_t* pArr = malloc(sizeof(palette_arr_t));
	pArr->numOfEntries = numOfPalettes;
	pArr->entries = malloc(numOfPalettes * sizeof(palette_t*));

	for (int i = 0; i < numOfPalettes; ++i) {
		pArr->entries[i] = NULL;
	}

	sys_list_dir((const char*)dir, OnSysDirList, pArr);

	return pArr;
}

static int OnSysDirList(const char *dir, const char *fname, void* data) {
	palette_arr_t* pArr = (palette_arr_t*) data;
	int pItems = pArr->numOfEntries;

	for (int i = 0; i < pItems; ++i) {
		if (pArr->entries[i] == NULL) {
			char fullPath[2048] = "";
			snprintf(fullPath, 2048, "%s/%s", dir, fname);

			FILE* fp = fopen(fullPath, "r");
			long int size = fsize(fp); // XX - Do Error (-1) Checking
			char* csvtxt = (char*) malloc(size * sizeof(char) + 1);
			fgets(csvtxt, size + 1, fp);
			pArr->entries[i] = LoadCsvPalette(csvtxt);

			free(csvtxt);
			fclose(fp);
			break;
		}
	}
	return 0;
}

static int OnAssetMgrList(int i, const char *fname) {
	FILE* file = NULL;
	const char* data = NULL;
	char* pallete_dir_path = getPaletteDirPath();
	char dir[CC_PATH_SIZE_MAX + 128] = "";
	snprintf(dir, CC_PATH_SIZE_MAX + 128, "%s/%s", pallete_dir_path, filenameFromPath(fname));

	data = assets_get(fname, NULL);
	file = fopen(dir, "wb");
	if (file) {
		fprintf(file, "%s", data);
		fclose(file);
	} else {
		log_error("cannot open file to write");
		return -1;
	}
	return 0;
}

static char* getPaletteDirPath() {
	char* configdir = CCGetConfigDir();
	static char configPath[CC_PATH_SIZE_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			log_error("cannot get the config directory!");
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "palettes");
			struct stat st = {0};
			if (stat(configPath, &st) == -1) {
				sys_make_dir(configPath);
			}
		} else {
			snprintf(configPath, CC_PATH_SIZE_MAX + 128, "%s/csprite/palettes", configdir);
			struct stat st = {0};
			if (stat(configPath, &st) == -1) {
				sys_make_dir(configPath);
			}
		}
	}

	return configPath;
}

static char* filenameFromPath(const char* path) {
	static char fileName[1024] = "";
	int pathLen = strlen(path);
	int lastSepIndex = 0;

	for (int i = 0; i < pathLen; ++i) {
		if (path[i] == '\\' || path[i] == '/') {
			lastSepIndex = i;
		}
	}

	lastSepIndex++;
	strncpy(fileName, path + lastSepIndex, pathLen - lastSepIndex);

	return fileName;
}
