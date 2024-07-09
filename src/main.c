#include <stdio.h>
#include "app/app.h"

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

