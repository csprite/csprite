#include "app/app.h"

int main(void) {
	OS_Handle window = app_init();
	app_main_loop(window);
	app_release(window);

	return 0;
}
