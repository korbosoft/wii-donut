#ifndef COLOR_H
#define COLOR_H

#include <gccore.h>

extern int RGB_escape(const u8 r, const u8 g, const u8 b, const bool fg);

extern GXColor generate_rad_noise();

extern GXColor HSV_to_RGB(float h, float s, float v);

#endif
