#ifndef I_FILE_IO_H_
#define I_FILE_IO_H_ 1

#include <stdint.h>
#include "../renderer/canvas.h"

// Full Support For Reading & Writing PNGs & JPEGs/JPGs
int32_t ifio_write(const char* filePath, CanvasLayer_Manager* mgr);
int32_t ifio_read(const char* filePath, CanvasLayer_Manager** mgr, Pixel& checkBg1, Pixel& checkBg2);

#endif // I_FILE_IO_H_

