#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct settings {
	bool vsync;
	bool CustomCursor;
	bool accelerated; // Hardware acceleration
	char renderer[128]; // Renderer To use eg: opengl, vulkan or metal.
};

typedef struct settings settings_t;

settings_t* LoadSettings(void);
int WriteSettings(settings_t* s);

#ifdef __cplusplus
}
#endif
