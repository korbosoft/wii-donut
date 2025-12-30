#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <math.h>

#include "color.h"

int RGB_escape(const u8 r, const u8 g, const u8 b, const bool fg) {
	if ((r > 255) || (g > 255) || (b > 255))
		return -1;
	printf("\e[%i;2;%i;%i;%im", fg ? 38 : 48, r, g, b);
	return 0;
}

GXColor generate_rad_noise() {
	u16 r, g, b;
	float mult = (float)rand() / (float)RAND_MAX;
	r = rand() % 256 * mult;
	if (r > 255) r = 255;
	g = rand() % 256 * mult / 2;
	if (g > 255) g = 255;
	b = rand() % 256 * mult;
	if (b > 255) b = 255;
	return (GXColor){r, g, b};
}

GXColor HSV_to_RGB(float h, float s, float v) {
	float r, g, b;
	float c = v * s;
	float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
	float m = v - c;

	if (h >= 0 && h < 60) {
		r = c;
		g = x;
		b = 0;
	} else if (h >= 60 && h < 120) {
		r = x;
		g = c;
		b = 0;
	} else if (h >= 120  && h < 180) {
		r = 0;
		g = c;
		b = x;
	} else if (h >= 180 && h < 240) {
		r = 0;
		g = x;
		b = c;
	} else if (h >= 240 && h < 300) {
		r = x;
		g = 0;
		b = c;
	} else {
		r = c;
		g = 0;
		b = x;
	}

	r += m;
	g += m;
	b += m;

	return (GXColor){roundf(r * 255), roundf(g * 255), roundf(b * 255)};
}
