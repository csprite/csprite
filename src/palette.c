#define _DEFAULT_SOURCE

#include <string.h>
#include <sys/stat.h>

#include "system.h"
#include "palette.h"
#include "logger.h"
#include "assets.h"

// Forward Declarations
static int OnSysDirList(const char *dir, const char *fname, void* data);
static int OnAssetMgrList(int i, const char *fname);

int FreePaletteArr(PaletteArr_T* pArr) {
	if (pArr == NULL) {
		Logger_Error("Palette Array Pointer is NULL!\n");
		return -1;
	}

	if (pArr->Palettes != NULL) {
		for (int32_t i = 0; i < pArr->numOfEntries; ++i) {
			FreePalette(pArr->Palettes[i]);
			pArr->Palettes[i] = NULL;
		}
		free(pArr->Palettes);
		pArr->Palettes = NULL;
	}

	free(pArr);
	return 0;
}


int FreePalette(Palette_T* palette) {
	if (palette == NULL) {
		Logger_Error("Palette Pointer is NULL!\n");
		return -1;
	}

	if (palette->Colors == NULL) {
		Logger_Error("Palette Entries are NULL!\n");
		return -1;
	}

	free(palette->Colors);
	free(palette);
	return 0;
}

Palette_T* LoadCsvPalette(const char* csvText) {
	Palette_T* newPalette = malloc(sizeof(Palette_T));
	if (newPalette == NULL) {
		Logger_Error("'malloc' returned NULL, cannot allocate memory for the palette!\n");
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
	newPalette->Colors = malloc(sizeof(palette_entry_t) * newPalette->numOfEntries);

	// First We Find Out Total Elements, To Allocate Array
	token = strtok(text, ","); // Palette Name
	strncpy(newPalette->name, token, PaletteNameSize - 1);
	token = strtok(NULL, ","); // Author Name
	memset(newPalette->author, 0, PaletteAuthorSize);
	snprintf(newPalette->author, PaletteAuthorSize, "Awesome Palette By %s", token == NULL ? "Unknown" : token);
	token = strtok(NULL, ","); // First Color

	// XX - Todo Fix When Author Name Or Palette Name is Not Present it glitches

	for (int i = 0; token != NULL; ++i) {
		unsigned int r = 0, g = 0, b = 0;
		sscanf(token, "%02x%02x%02x", &r, &g, &b);
		newPalette->Colors[i][0] = r;
		newPalette->Colors[i][1] = g;
		newPalette->Colors[i][2] = b;
		newPalette->Colors[i][3] = 255;
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

char* SysGetPaletteDir() {
	char* configdir = Sys_GetConfigDir();
	static char configPath[SYS_PATHNAME_MAX + 128] = "";

	if (!*configPath) {
		if (configdir == NULL) {
			Logger_Warn("cannot get the config directory!\n");
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "palettes");
			Sys_MakeDirRecursive(configPath);
		} else {
			snprintf(configPath, SYS_PATHNAME_MAX + 128, "%s%ccsprite%cpalettes", configdir, SYS_PATH_SEP, SYS_PATH_SEP);
			Sys_MakeDirRecursive(configPath);
		}
	}

	return configPath;
}

PaletteArr_T* PaletteLoadAll() {
	char* pallete_dir_path = SysGetPaletteDir();
	char dir[SYS_PATHNAME_MAX + 128] = "";
	strncpy(dir, pallete_dir_path, SYS_PATHNAME_MAX);

	// Check If Palettes Directory On Disk Is Empty, If So Extract The Built-in Palettes
	int numOfPalettes = Sys_ListDirContents((const char*)dir, NULL, NULL);
	if (numOfPalettes <= 0) {
		Sys_MakeDirRecursive(dir);
		Assets_List("data/palettes/", OnAssetMgrList);
	}

	numOfPalettes = Sys_ListDirContents((const char*)dir, NULL, NULL);
	if (numOfPalettes <= 0) {
		Logger_Error("cannot extract the palettes!\n");
		return NULL;
	}

	PaletteArr_T* pArr = malloc(sizeof(PaletteArr_T));
	pArr->numOfEntries = numOfPalettes;
	pArr->Palettes = malloc(numOfPalettes * sizeof(Palette_T*));

	for (int i = 0; i < numOfPalettes; ++i) {
		pArr->Palettes[i] = NULL;
	}

	Sys_ListDirContents((const char*)dir, OnSysDirList, pArr);

	return pArr;
}

static int OnSysDirList(const char *dir, const char *fname, void* data) {
	PaletteArr_T* pArr = (PaletteArr_T*) data;
	int pItems = pArr->numOfEntries;

	for (int i = 0; i < pItems; ++i) {
		if (pArr->Palettes[i] == NULL) {
			char fullPath[2048] = "";
			snprintf(fullPath, 2048, "%s/%s", dir, fname);

			FILE* fp = fopen(fullPath, "r");
			size_t size = Sys_GetFileSize(fp); // XX - Do Error (-1) Checking
			char* csvtxt = (char*) malloc((size + 1) * sizeof(char));
			memset(csvtxt, '\0', size + 1);
			fread(csvtxt, size + 1, 1, fp);
			pArr->Palettes[i] = LoadCsvPalette(csvtxt);

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
	char* pallete_dir_path = SysGetPaletteDir();
	char dir[SYS_PATHNAME_MAX + 128] = "";
	char* fileName = Sys_GetBasename(fname);
	snprintf(dir, SYS_PATHNAME_MAX + 128, "%s/%s", pallete_dir_path, fileName);

	free(fileName);
	fileName = NULL;

	data = Assets_Get(fname, NULL);
	file = fopen(dir, "wb");
	if (file) {
		fprintf(file, "%s", data);
		fclose(file);
	} else {
		Logger_Error("cannot open file to write\n");
		return -1;
	}
	return 0;
}
