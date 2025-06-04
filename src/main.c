#include "app/app.h"

int main(void) {
	app_init();
	app_main_loop();
	app_deinit();
	return 0;
}
