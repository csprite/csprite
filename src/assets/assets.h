#ifndef CSP_EMBEDDED_ASSETS_MANAGER_H
#define CSP_EMBEDDED_ASSETS_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

// All the assets are saved in binary directly in the code, using tool/create_assets.py.

/*
	Function: assets_get()
	Arguments:
		filePath: ex - data/fonts/Montserrat-Bold.ttf
		size: size of the data will be stored in this variable
*/
const void* assets_get(const char *filePath, int *size);

/*
	Function: assets_list()
	Description: List all the assets in a given asset dir,
				 returns the number of assets listed, calls
				 the callback function on every item.
				 if the callback doesn't return 0, the asset
				 is skipped.
	Arguments:
		directoryPath: directory to list items of
		callback: function to call on every item
*/
int assets_list(const char* directoryPath, int (*callback)(int i, const char *path));

#ifdef __cplusplus
}
#endif

#endif // CSP_EMBEDDED_ASSETS_MANAGER_H

