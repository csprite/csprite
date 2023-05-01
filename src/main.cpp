#include <stdio.h>

int main(void) {
#ifdef _DEBUG
	printf("Hello Debug!\n");
#else
	printf("Hello Release!\n");
#endif
	return 0;
}
