#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int CreateShaderProgram(const char* vertPath, const char* fragPath, const char* geomPath);

#ifdef __cplusplus
}
#endif

#endif
