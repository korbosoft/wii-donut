#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "donut.h"

#include "text.h"

typedef struct {
	char image[SCREEN_WIDTH][SCREEN_HEIGHT];
	float zBuffer[SCREEN_WIDTH][SCREEN_HEIGHT];
	float depthMap[SCREEN_WIDTH][SCREEN_HEIGHT];
	bool isDough[SCREEN_WIDTH][SCREEN_HEIGHT];
} Render;

float hue_offset = 0;
void render_frame(float A, float B, Donut flavor) {
	const u8 R1 = 1, R2 = 2, K2 = 5;

	const float K1 = SCREEN_HEIGHT * K2 * 3 / (8 * (R1+R2));
	Render render;

	// precompute sines and cosines of A and B
	const float cosA = cos(A), sinA = sin(A);
	const float cosB = cos(B), sinB = sin(B);
	memset(render.image, ' ', sizeof(char) * SCREEN_SIZE);
	memset(render.zBuffer, 0, sizeof(float) * SCREEN_SIZE);

	// theta goes around the cross-sectional circle of a torus
	for (float theta=0; theta < PI_TIMES_2; theta += THETA_SPACING) {
		// precompute sines and cosines of theta
		const float costheta = cos(theta), sintheta = sin(theta);

		// phi goes around the center of revolution of a torus
		for (float phi=0; phi < PI_TIMES_2; phi += PHI_SPACING) {
			// precompute sines and cosines of phi
			const float cosphi = cos(phi), sinphi = sin(phi);

			// the x,y coordinate of the circle, before revolving (factored
			// out of the above equations)
			const float circlex = R2 + R1 * costheta;
			const float circley = R1 * sintheta;

			// final 3D (x,y,z) coordinate after rotations, directly from
			// our math above
			const float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
			const float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
			const float z = K2 + cosA * circlex * sinphi + circley * sinA;
			const float ooz = 1 / z;  // "One Over Z"

			// x and y projection.  note that y is negated here, because y
			// goes up in 3D space but down on 2D displays.
			// EDIT: x multiplied by 2 to account for 8x16 characters -- Korbo
			const u8 xp = (u8) (SCREEN_WIDTH / 2 + K1 * ooz * x * 2);
			const u8 yp = (u8) (SCREEN_HEIGHT / 2 - K1 * ooz * y);

			// calculate luminance.  ugly, but correct.
			const float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);
			// L ranges from -sqrt(2) to +sqrt(2).  If it's < 0, the surface
			// is pointing away from us, so we won't bother trying to plot it.
			// EDIT: fuck you i'm plotting it anyways -- Korbo
			if (flavor.flags.ghost ? L <= 0 : L > 0) {
				// test against the z-buffer.  larger 1/z means the pixel is
				// closer to the viewer than what's already plotted.
				// printf("xp: %d, yp: %d\n", xp, yp);

				if (ooz > render.zBuffer[xp][yp]) {
					render.zBuffer[xp][yp] = ooz;
					const s8 luminance_index = L * (5 / M_SQRT2);
					// const int wave = theta < M_PI ? (M_PI - (sin(phi * 8))) : -(M_PI - (sin(phi * 8)));
					float wave = (0.5 - sin(phi * 12)) / 5;
					// if (wave >= 3) wave = -wave;
					render.isDough[xp][yp] = (wave > theta) || (theta > M_PI + wave);
					render.depthMap[xp][yp] = L / M_SQRT2;
					// luminance_index is now in the range 0..5 (8*sqrt(2) = 11.3)
					// now we lookup the character corresponding to the
					// luminance and plot it in our output:
					render.image[xp][yp] = ".;>fM"[flavor.flags.ghost ? luminance_index + 4 : luminance_index];
				}
			}
		}
	}

	// now, dump output[] to the screen.
	// bring cursor to "home" location
	print("\e[2;0H");
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			print("\e[0;0m");
			if (render.image[i][j] != ' ') {
				if (flavor.flags.radiates) {
					if (rand() % 3) {
						GXColor radiatedBg = generate_rad_noise();
						RGB_escape(
							radiatedBg.r,
							radiatedBg.g,
							radiatedBg.b, false
						);
					}
				}
				if (flavor.flags.rainbow) {
					float hue = render.depthMap[i][j] * 360 + hue_offset;
					if (hue > 360) hue -= 360;
					GXColor rainbow = HSV_to_RGB(hue, 1, 1);
					RGB_escape(rainbow.r, rainbow.g, rainbow.b, true);
				} else if (render.isDough[i][j]) {
					RGB_escape(flavor.bottom.r, flavor.bottom.g, flavor.bottom.b, true);
				} else {
					RGB_escape(flavor.top.r, flavor.top.g, flavor.top.b, true);
				}
			}
			putchar(render.image[i][j]);
		}
		putchar('\n');
		printf("\e[%u;0H", j + 2);

	}
	hue_offset = fmod(hue_offset + 1, 360);
}
