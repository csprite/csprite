#include "app/app.h"
#include "base/math.h"
#include "base/string.h"
#include "os/os.h"
#include "base/types.h"
#include <stdio.h>

int main(void) {
	app_init();
	app_main_loop();
	app_deinit();
	return 0;
}
