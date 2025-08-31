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

#define PI_TIMES_2 6.2831853071795865
#define SPLASH_COUNT 5
#define SCREEN_WIDTH 77
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
	const float cosA = cos(A), sinA = sin(A);
	const float cosB = cos(B), sinB = sin(B);
	memset(output, ' ', sizeof(char) * SCREEN_SIZE);
	memset(zbuffer, 0, sizeof(float) * SCREEN_SIZE);

	// theta goes around the cross-sectional circle of a torus
	for (float theta=0; theta < PI_TIMES_2; theta += theta_spacing) {
		// precompute sines and cosines of theta
		const float costheta = cos(theta), sintheta = sin(theta);

		// phi goes around the center of revolution of a torus
		for (float phi=0; phi < PI_TIMES_2; phi += phi_spacing) {
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
			const float ooz = 1 / z;  // "one over z"

			// x and y projection.  note that y is negated here, because y
			// goes up in 3D space but down on 2D displays.
			const int xp = (int) (SCREEN_WIDTH / 2 + K1*ooz*x*2); // multiplied by 2 to make wide again --Korbo
			const int yp = (int) (SCREEN_HEIGHT / 2 - K1*ooz*y);

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

	// now, dump output[] to the screen.
	// bring cursor to "home" location
	print("\e[2;0H");
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			putchar(output[i][j]);
		}
		putchar('\n');
		printf("\e[%u;0H", j+2);

	}

}

void send_donut(void) {
	bool prev_paused = paused;
	music_pause(true);

	print("\e[0m\e[40;37m" "\e[23;0H" "\e[104m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \e[4mGBA Donut\e[0m\e[104;37m ┌─┐ Connect your GBA to controller port       Press + to cancel ║"
	"║           │Θ│ 2 with a GBA link cable.             ╒═──═╕                 ║"
	"║           │2│                                      │+░░∞│                 ║"
	"║           └─┘                                      └────┘      Waiting... ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m");

	while (wait_for_gba()) {
		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS) {
			music_pause(!prev_paused);
			return;
		}
	}

	print("\e[40m" "\e[23;0H" "\e[104m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \e[4mGBA Donut\e[0m\e[104;37m ┌─┐╔═══════════════════════════════════════╗                    ║"
	"║           │Θ╪╝                                     ╒═╨─═╕                 ║"
	"║           │2│                                      │+▒▒∞│                 ║"
	"║           └─┘                                      └────┘ Transferring... ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m");

	send_rom();

	print("\e[40m" "\e[23;0H" "\e[104m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \e[4mGBA Donut\e[0m\e[104;37m ┌─┐╔══════════════════√════════════════════╗                    ║"
	"║           │Θ╪╝                                     ╒═╨─═╕                 ║"
	"║           │2│                                      │+▓▓∞│                 ║"
	"║           └─┘                                      └────┘        Success! ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m");

	sleep(3);
	music_pause(!prev_paused);
}

const char *const splashMessages[SPLASH_COUNT] = {
	[0] = "Also try DS Donut!",
	[1] = "Better than Wii Donut! [citation needed]",
	[2] = "oh man please to help i am not good with c",
	[3] = "(\"Doughnut\" if you're british)",
	[4] = "Korbo loves you <3",
};

//---------------------------------------------------------------------------------
int main() {
	bool showControls = false;
	char splash[43], title[82];

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
	console_init(xfb,20,20,rmode->fbWidth-20,rmode->xfbHeight-20,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Clear the framebuffer
	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	file_init();

	srand(time(NULL));

	if (rand() % 49) {
		format_splash(splashMessages[rand() % SPLASH_COUNT] ?: "FLAGRANT SPLASH ERROR", splash);
	} else {
		format_splash("This splash has a 1/50 chance of appearing", splash);
	}


	print("\e[2;0H" "\e[37m" "\e[2J");

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
			showControls = !showControls;
		} else if (pressed & WPAD_BUTTON_A) {
			color++;
			color %= 7;
		} else if (pressed & WPAD_BUTTON_B) {
			music_pause(paused);
			paused = !paused;
		}
		printf("\e[0m" "\e[4%um", color);
		render_frame(A, B);
		A += 0.07;
		B += 0.02;
		if (showControls) {
			print("\e[23;0H" "\e[104m"
			"╔═══════════════════════════════════════════════════════════════════════════╗"
			"║ A    - Change BG color                                           Controls ║"
			"║ B    - Toggle music                                                       ║"
			"║ 1    - Send GBA Donut                                                     ║"
			"║ HOME - Exit                                           Press 2 to go back. ║"
			"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m");
		} else {
			printf("\e[23;0H" "\e[104m"
			"╔═══════════════════════════════════════════════════════════════════════════╗"
			"║ \e[4mKorbo's Wii Donut Mod %s   %s\e[0m\e[104;37m "                      "║"
			"║ Based on the original donut.c by Andy Sloane <andy@a1k0n.net>             ║"
			"║ Ported by emilydaemon <emilydaemon@donut.eu.org>, Modified by Korbo       ║"
			"║ Default Music by Jogeir Liljedahl                   Press 2 for controls. ║"
			"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m", VERSION, splash);
		}
		// printf("cwd: %s\n", getcwd(NULL, 0));
		printf("\e[0;0H" "\e[0m\e[4%um" "%s", color, title);
		printf("\e[0m\e[4%um", color);
		VIDEO_WaitVSync();
	} while (true);
	music_free();
	return 0;
}
