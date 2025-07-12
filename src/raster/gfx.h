#ifndef SRC_RASTER_GFX_H
#define SRC_RASTER_GFX_H 1
#pragma once

#include "raster/raster.h"
#include "raster/math.h"

// NOTE(pegvin) - Drawing Functions Take A S32 Vector, Meaning Positions Can
// Be Negative. All The Required Clipping & Bounds Check Are Done Under The Hood.

Region rs_gfx_draw_line(Raster* r, RGBAU8 color, Vec2S32 p1, Vec2S32 p2);
Region rs_gfx_draw_circle(Raster* r, RGBAU8 color, Vec2S32 center, U32 radius, B32 is_filled);
Region rs_gfx_draw_rect(Raster* r, RGBAU8 color, Vec2S32 p1, Vec2S32 p2);
Region rs_gfx_draw_ellipse(Raster* r, RGBAU8 color, Vec2S32 p1, Vec2S32 p2);

#endif // SRC_RASTER_GFX_H
