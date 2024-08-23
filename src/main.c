#include "app/app.h"
#include "fs/fs.h"

int main(void) {
	if (AppInit()) {
		return 1;
	}
	if (AppMainLoop()) {
		return 1;
	}

	AppDestroy();

	return 0;
}
