#include "assets.h"
#include "utils.h"
#include <stdio.h>

typedef struct {
	const char*     path;
	int             size;
	const void*     data __attribute__((aligned(4)));
} asset_t;

static asset_t ASSETS[]; // Defined in assets.inl

const void* Assets_Get(const char *filePath, int *size) {
	int i;
	if (StringStartsWith("asset://", filePath) == true) filePath += 8; // Skip asset://
	for (i = 0; ASSETS[i].path; i++) {
		if (strcmp(ASSETS[i].path, filePath) == 0) {
			if (size) *size = ASSETS[i].size;
			return ASSETS[i].data;
		}
	}
	return NULL;
}

int Assets_List(const char* directoryPath, int (*callback)(int i, const char *path)) {
	int i, j = 0;
	for (i = 0; ASSETS[i].path; i++) {
		if (StringStartsWith(directoryPath, ASSETS[i].path)) {
			if (callback != NULL) {
				if (callback(j, ASSETS[i].path) == 0)
					j++;
			}
		}
	}
	return j;
}

static asset_t ASSETS[] = {
	#include "assets/palettes.inl"
	#include "assets/themes.inl"
	#include "assets/fonts.inl"
	#include "assets/icons.inl"
	{ NULL, 0, NULL } // NULL asset at the end of the list.
};
