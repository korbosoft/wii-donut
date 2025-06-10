#include <gccore.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "file.h"
#include "goombasend.h"
#include "music.h"
#include "text.h"

#define TAU 6.2831853071795864
#define SPLASH_COUNT 5
#define SCREEN_WIDTH 75
#define SCREEN_HEIGHT 22
#define SCREEN_SIZE SCREEN_WIDTH * SCREEN_HEIGHT
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

u8 *resbuf, *cmdbuf;
volatile u32 transval = 0;
volatile u32 resval = 0;

static bool paused = true;

void render_frame(float A, float B) {
	const float theta_spacing = 0.07;
	const float phi_spacing = 0.02;

	const int R1 = 1;
	const int R2 = 2;
	const int K2 = 5;

	const float K1 = SCREEN_HEIGHT*K2*3/(8*(R1+R2));
	char output[SCREEN_WIDTH][SCREEN_HEIGHT];
	float zbuffer[SCREEN_WIDTH][SCREEN_HEIGHT];


	// precompute sines and cosines of A and B
	float cosA = cos(A), sinA = sin(A);
	float cosB = cos(B), sinB = sin(B);
	memset(output, ' ', sizeof(char) * SCREEN_SIZE);
	memset(zbuffer, 0, sizeof(float) * SCREEN_SIZE);

	// theta goes around the cross-sectional circle of a torus
	for (float theta=0; theta < TAU; theta += theta_spacing) {
		// precompute sines and cosines of theta
		float costheta = cos(theta), sintheta = sin(theta);

		// phi goes around the center of revolution of a torus
		for (float phi=0; phi < TAU; phi += phi_spacing) {
			// precompute sines and cosines of phi
			float cosphi = cos(phi), sinphi = sin(phi);

			// the x,y coordinate of the circle, before revolving (factored
			// out of the above equations)
			float circlex = R2 + R1 * costheta;
			float circley = R1 * sintheta;

			// final 3D (x,y,z) coordinate after rotations, directly from
			// our math above
			float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
			float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
			float z = K2 + cosA * circlex * sinphi + circley * sinA;
			float ooz = 1 / z;  // "one over z"

			// x and y projection.  note that y is negated here, because y
			// goes up in 3D space but down on 2D displays.
			int xp = (int) (SCREEN_WIDTH / 2 + K1*ooz*x);
			int yp = (int) (SCREEN_HEIGHT / 2 - K1*ooz*y);

			// calculate luminance.  ugly, but correct.
			float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);
			// L ranges from -sqrt(2) to +sqrt(2).  If it's < 0, the surface
			// is pointing away from us, so we won't bother trying to plot it.
			if (L > 0) {
				// test against the z-buffer.  larger 1/z means the pixel is
				// closer to the viewer than what's already plotted.
				// printf("xp: %d, yp: %d\n", xp, yp);

				if (ooz > zbuffer[xp][yp]) {
					zbuffer[xp][yp] = ooz;
					int luminance_index = L * 8;
					// luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
					// now we lookup the character corresponding to the
					// luminance and plot it in our output:
					output[xp][yp] = ".,-~:;=!*#$@"[luminance_index];
				}
			}
		}
	}

	// now, dump output[] to the screen.
	// bring cursor to "home" location
	print("\x1b[1;1H");
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			putchar(output[i][j]);
		}
		putchar('\n');
		printf("\x1b[%u;1H", j+1);

	}

}

void send_donut(void) {
	bool prev_paused = paused;
	music_pause(true);

	print("\x1b[40m\x1b[22;0H"
	" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
	" \x1b[44m║ GBA Donut ┌─┐ Connect your GBA to controller          Press + to cancel ║\x1b[40m "
	" \x1b[44m║           │Θ│ port 2 with a GBA link cable.      ╒═──═╕                 ║\x1b[40m "
	" \x1b[44m║           │2│                                    │+░░∞│                 ║\x1b[40m "
	" \x1b[44m║           └─┘                                    └────┘      Waiting... ║\x1b[40m "
	" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");

	while (wait_for_gba()) {
		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS) {
			music_pause(!prev_paused);
			return;
		}
	}

	print("\x1b[22;0H\x1b[40m"
	" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
	" \x1b[44m║ GBA Donut ┌─┐╔═════════════════════════════════════╗                    ║\x1b[40m "
	" \x1b[44m║           │Θ╪╝                                   ╒═╨─═╕                 ║\x1b[40m "
	" \x1b[44m║           │2│                                    │+▒▒∞│                 ║\x1b[40m "
	" \x1b[44m║           └─┘                                    └────┘ Transferring... ║\x1b[40m "
	" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");

	send_rom();

	print("\x1b[22;0H\x1b[40m"
	" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
	" \x1b[44m║ GBA Donut ┌─┐╔══════════════════√══════════════════╗                    ║\x1b[40m "
	" \x1b[44m║           │Θ╪╝                                   ╒═╨─═╕                 ║\x1b[40m "
	" \x1b[44m║           │2│                                    │+▓▓∞│                 ║\x1b[40m "
	" \x1b[44m║           └─┘                                    └────┘        Success! ║\x1b[40m "
	" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");

	sleep(3);
	music_pause(!prev_paused);
}

const char* const splashMessages[SPLASH_COUNT] = {
	[0] = "                      Also try DS Donut!",
	[1] = "Better than Wii Donut! [citation needed]",
	[2] = "oh man please to help i am not good with",
	[3] = "          (\"Doughnut\" if you're british)",
	[4] = "                         Korbo loves you",
};

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
	bool showMore = false;
	char splash[41], title[27];

	//---------------------------------------------------------------------------------
	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Initialise the audio subsystem
	AESND_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if (rmode->viTVMode&VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	fatInitDefault();

	srand(time(NULL));

	strcpy(splash, splashMessages[rand() % SPLASH_COUNT] ?: "FLAGRANT SPLASH ERROR");

	print("\x1b[2;0H" "\x1b[40;37m" "\x1b[2J");

	music_init(title);

	prepare_rom();

	float A = 1, B = 1;
	u8 color = 0;
	do {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if (pressed & WPAD_BUTTON_HOME) {
			break;
		} else if (pressed & WPAD_BUTTON_1) {
			send_donut();
		} else if (pressed & WPAD_BUTTON_2) {
			showMore = !showMore;
		} else if (pressed & WPAD_BUTTON_A) {
			color++;
			color %= 7;
		} else if (pressed & WPAD_BUTTON_B) {
			music_pause(paused);
			paused = !paused;
		}
		printf("\x1b[4%um", color);
		render_frame(A, B);
		A += 0.07;
		B += 0.03;
		if (showMore) {
			print("\x1b[22;0H" "\x1b[40m"
			" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
			" \x1b[44m║ A    - Change BG color                                                  ║\x1b[40m "
			" \x1b[44m║ B    - Toggle music                                                     ║\x1b[40m "
			" \x1b[44m║ 1    - Send GBA Donut                                                   ║\x1b[40m "
			" \x1b[44m║ HOME - Exit                                         Press 2 to go back. ║\x1b[40m "
			" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");
		} else {
			printf("\x1b[22;0H" "\x1b[40m"
			" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
			" \x1b[44m║ Korbo's Wii Donut Mod %s   %s ║\x1b[40m "
			" \x1b[44m║ Based on the original donut.c by Andy Sloane <andy@a1k0n.net>           ║\x1b[40m "
			" \x1b[44m║ Ported by emilydaemon <emilydaemon@donut.eu.org>, Modified by Korbo     ║\x1b[40m "
			" \x1b[44m║ Default Music by Jogeir Liljedahl                Press 2 for more info. ║\x1b[40m "
			" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m", VERSION, splash);
		}
		print("\x1b[0;1H");
		// printf("cwd: %s\n", getcwd(NULL, 0));
		printf("\x1b[4%um%s", color, title);
		VIDEO_WaitVSync();
	} while (true);
	GRRMOD_End();
	return 0;
}
