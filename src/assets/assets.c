#include <string.h>

#include "assets/assets.h"

typedef unsigned char uint8_t;

typedef struct {
	long        size;
	const char* path;
#ifdef TARGET_WINDOWS
	__declspec(align(4)) const void*     data;
#else
	const void* data __attribute__((aligned(4)));
#endif
} asset_t;

static asset_t ASSETS[] = {
	#include "assets/assets.inl"
};

#define NUM_ASSETS (int)(sizeof(ASSETS) / sizeof(asset_t))

const void* assets_get(const char *filePath, int *size) {
	for (int i = 0; i < NUM_ASSETS; i++) {
		if (strcmp(ASSETS[i].path, filePath) == 0) {
			if (size) *size = ASSETS[i].size;
			return ASSETS[i].data;
		}
	}
	return NULL;
}

#define STARTS_WITH(prefix, str) (strncmp(prefix, str, strlen(prefix)) == 0)

int assets_list(const char* directoryPath, int (*callback)(int i, const char *path)) {
	int j = 0;
	for (int i = 0; i < NUM_ASSETS; i++) {
		if (STARTS_WITH(directoryPath, ASSETS[i].path)) {
			if (!callback || callback(j, ASSETS[i].path) == 0) j++;
		}
	}
	return j;
}

