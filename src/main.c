#include "app/app.h"
#include "fs/fs.h"

int main(void) {
	if (app_init()) {
		return 1;
	}
	if (app_main_loop()) {
		return 1;
	}

	app_destroy();

	return 0;
}
