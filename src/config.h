#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	bool vsync;
} Config_T;

Config_T* LoadConfig(void);
int WriteConfig(Config_T* s);

#ifdef __cplusplus
}
#endif
