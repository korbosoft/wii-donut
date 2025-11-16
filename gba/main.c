#include <gba.h>
#include <gba_systemcalls.h>

#include <string.h>
#include <stdio.h>
#include <math.h>

#define DONUT_WIDTH 14
#define DONUT_HEIGHT 14
#define DONUT_SIZE (DONUT_WIDTH * DONUT_HEIGHT)
#define FRAME_COUNT 39
#define PI_TIMES_2 6.2831853071795865

#define CURRENT_LINE (*(volatile unsigned short *)0x4000006)

void render_frame(float A, float B, char output[DONUT_WIDTH][DONUT_HEIGHT]) {
	const float theta_spacing = 0.07;
	const float phi_spacing = 0.02;

	const int R1 = 1;
	const int R2 = 2;
	const int K2 = 5;

	const float K1 = DONUT_HEIGHT*K2*3/(8*(R1+R2));
	float zbuffer[DONUT_WIDTH][DONUT_HEIGHT];


	// precompute sines and cosines of A and B
	const float cosA = cos(A), sinA = sin(A);
	const float cosB = cos(B), sinB = sin(B);
	memset(output, ' ', sizeof(char) * DONUT_SIZE);
	memset(zbuffer, 0, sizeof(float) * DONUT_SIZE);

	// theta goes around the cross-sectional circle of a torus
	for (float theta=0; theta < PI_TIMES_2; theta += theta_spacing) {
		// precompute sines and cosines of theta
		const float costheta = cos(theta), sintheta = sin(theta);

		// phi goes around the center of revolution of a torus
		for (float phi=0; phi < PI_TIMES_2; phi += phi_spacing) {
			// precompute sines and cosines of phi
			const float cosphi = cos(phi), sinphi = sin(phi);

			// the x,y coordinate of the circle, before revolving (factoredc
			// out of the above equations)
			const float circlex = R2 + R1 * costheta;
			const float circley = R1 * sintheta;

			// final 3D (x,y,z) coordinate after rotations, directly from
			// our math above
			const float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
			const float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
			const float z = K2 + cosA * circlex * sinphi + circley * sinA;
			const float ooz = 1 / z;  // "one over z"

			// x and y projection.  note that y is negated here, because y
			// goes up in 3D space but down on 2D displays.
			const int xp = (int) (DONUT_WIDTH / 2 + K1*ooz*x);
			const int yp = (int) (DONUT_HEIGHT / 2 - K1*ooz*y);

			// calculate luminance.  ugly, but correct.
			const float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);
			// L ranges from -sqrt(2) to +sqrt(2).  If it's < 0, the surface
			// is pointing away from us, so we won't bother trying to plot it.
			if (L > 0) {
				// test against the z-buffer.  larger 1/z means the pixel is
				// closer to the viewer than what's already plotted.
				// printf("xp: %d, yp: %d\n", xp, yp);

				if (ooz > zbuffer[xp][yp]) {
					zbuffer[xp][yp] = ooz;
					const int luminance_index = L * 8;
					// luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
					// now we lookup the character corresponding to the
					// luminance and plot it in our output:
					output[xp][yp] = ".,-~:;=!*#$@"[luminance_index];
				}
			}
		}
	}

	// // now, dump output[] to the screen.
	// // bring cursor to "home" location
	// print("\e[2;0H");
	// for (int j = 0; j < DONUT_HEIGHT; j++) {
	//     for (int i = 0; i < DONUT_WIDTH; i++) {
	//         putchar(output[i][j]);
	//     }
	//     putchar('\n');
	//     printf("\e[%u;0H", j+2);
	//
	// }

}

void wait(int x) {
	for (int i = 1; i < x; i++) {VBlankIntrWait();}
}

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleInit(0, 4, 0, NULL, 0, 15);
	BG_COLORS[0]=RGB8(0,0,0);
	BG_COLORS[241]=RGB5(31,31,31);
	SetMode(MODE_0 | BG0_ON);

	char donutBuffer[FRAME_COUNT][DONUT_WIDTH][DONUT_HEIGHT];
	float A = 1, B = 1;

	for (int i = 0; i < FRAME_COUNT; i++) {
		iprintf("\x1b[2JCalculating frame %i/%i...\nThis takes about 48 seconds\nper frame.", i+1, FRAME_COUNT);
		render_frame(A, B, donutBuffer[i]);
		A += 0.56;
		B += 0.16;

	}

	puts("\x1b[2J");
	iprintf("\x1b[14;0H"
			".----------------------------." \
			"| GBA Donut v2.0.1           |" \
			"| Ported by korbosoft        |" \
			"|                            |" \
			"|    (Turn off GBA to exit.) |" \
			"'----------------------------'");
	do {
		for (int k = 0; k < FRAME_COUNT; k++) {
			printf("\x1b[0;0H");
			for (int j = 0; j < DONUT_HEIGHT; j++) {
				iprintf("\x1b[8C");
				for (int i = 0; i < DONUT_WIDTH; i++)
					putchar(donutBuffer[k][i][j]);
				iprintf("\n");
			}
			iprintf("\x1b[0;0H");
			wait(5);
		}
	} while( 1 );
}
