#include "app/app.h"
#include "fs/fs.h"

int main(void) {
	app_init();
	app_main_loop();
	app_deinit();
	return 0;
}
