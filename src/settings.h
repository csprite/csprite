#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct settings {
	bool vsync;
	char renderer[128];
};

typedef struct settings settings_t;

settings_t* LoadSettings(void);
int WriteSettings(settings_t* s);

#ifdef __cplusplus
}
#endif
