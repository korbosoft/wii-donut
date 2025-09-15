#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	u8 r, g, b;
} RGB;

static const RGB colorTable[] = {
	{16, 16, 16}, // black
	{117, 19, 20}, // red
	{23, 121, 24}, // green
	{124, 126, 27}, // yellow
	{18, 16, 115}, // blue
	{119, 21, 118}, // magenta
	{25, 123, 122}, // cyan
	{192, 192, 192}, // white
};

int RGB_escape(const u8 r, const u8 g, const u8 b, const bool fg) {
	if ((r > 255) || (g > 255) || (b > 255))
		return -1;
	printf("\e[%i;2;%i;%i;%im", fg ? 38 : 48, r, g, b);
	return 0;
}

const RGB generate_rad_noise(u8 bgColor) {
	u16 r, g, b;
	float mult = (float)rand() / (float)RAND_MAX;
	RGB bg = colorTable[bgColor];
	r = rand() % 256 * mult + bg.r;
	if (r > 255) r = 255;
	g = rand() % 256 * mult + bg.g;
	if (g > 255) g = 255;
	b = rand() % 256 * mult + bg.b;
	if (b > 255) b = 255;
	return (RGB){r, g, b};
}

RGB HSV_to_RGB(float h, float s, float v) {
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

	return (RGB){roundf(r * 255), roundf(g * 255), roundf(b * 255)};
}

#endif
