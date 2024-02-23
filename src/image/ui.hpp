#ifndef CSP_SRC_IMAGE_UI_HPP_INCLUDED_
#define CSP_SRC_IMAGE_UI_HPP_INCLUDED_ 1
#pragma once

#include "imgui/imgui.h"
#include "image/image.hpp"

// Returns True as a Request to re-render the whole document
bool ImageLayers_UI_Draw(Image& img, u32& activeLayer, bool& ShowLayerPropertiesWindow);

#endif // CSP_SRC_IMAGE_UI_HPP_INCLUDED_
