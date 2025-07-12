#include "raster/math.h"

void v2s32_ensure_tl_br(Vec2S32* min, Vec2S32* max) {
	if (min->x > max->x) {
		Swap(S32, min->x, max->x);
	}
	if (min->y > max->y) {
		Swap(S32, min->y, max->y);
	}
}

void v2u32_ensure_tl_br(Vec2U32* min, Vec2U32* max) {
	if (min->x > max->x) {
		Swap(S32, min->x, max->x);
	}
	if (min->y > max->y) {
		Swap(S32, min->y, max->y);
	}
}

Rect region_to_rect(Region r) {
	v2u32_ensure_tl_br(&r.min, &r.max);
	return rect(r.max.x - r.min.x + 1, r.max.y - r.min.y + 1);
}

Region region_clipped(Rect bounds, Vec2S32 min, Vec2S32 max) {
	v2s32_ensure_tl_br(&min, &max);

	Region r = {
		.min = {
			.x = min.x < 0 ? 0 : min.x,
			.y = min.y < 0 ? 0 : min.y
		},
		.max = {
			.x = max.x >= (S32)bounds.w ? bounds.w - 1 : (U32)max.x,
			.y = max.y >= (S32)bounds.h ? bounds.h - 1 : (U32)max.y
		},
	};

	return r;
}

Region region_clipped_xy_wh(Rect bounds, Vec2S32 xy, Rect wh) {
	return region_clipped(bounds, xy, v2s32(xy.x + wh.w, xy.y + wh.h));
}
