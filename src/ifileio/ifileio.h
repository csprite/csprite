#ifndef I_FILE_IO_H_
#define I_FILE_IO_H_ 1

#include <stdint.h>
#include "../renderer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif

// Full Support For Reading & Writing PNGs & JPEGs/JPGs
int32_t ifio_write(const char* filePath, int32_t w, int32_t h, CanvasLayerArr_T* arr);
int32_t ifio_read(const char* filePath, int32_t* w_ptr, int32_t* h_ptr, CanvasLayerArr_T** arr);

#ifdef __cplusplus
}
#endif

#endif // I_FILE_IO_H_

