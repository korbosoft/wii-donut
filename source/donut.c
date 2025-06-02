#include <aesndlib.h>
#include <gccore.h>
#include <gcmodplay.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "goombasend.h"

#include "music_mod.h"

#define TAU 6.2831853071795864
#define SPLASH_COUNT 5

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
static MODPlay play;

bool senddemo = false;
u8 *resbuf, *cmdbuf;
volatile u32 transval = 0;
volatile u32 resval = 0;

void sendDonut() {
	MODPlay_Pause(&play, true);
	printf("\x1b[22;0H\x1b[40m"
	" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
	" \x1b[44m║ GBA Donut ┌─┐                                         Press + to cancel ║\x1b[40m "
	" \x1b[44m║           │Θ│                                    ╒═──═╕                 ║\x1b[40m "
	" \x1b[44m║           │2│                                    │+▒▒∞│                 ║\x1b[40m "
	" \x1b[44m║           └─┘                                    └────┘      Waiting... ║\x1b[40m "
	" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");
	while (wait_for_gba() != 0) {
		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS)
			return;
	}
	printf("\x1b[22;0H\x1b[40m"
	" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
	" \x1b[44m║ GBA Donut ┌─┐╔═════════════════════════════════════╗                    ║\x1b[40m "
	" \x1b[44m║           │Θ╪╝                                   ╒═╨─═╕                 ║\x1b[40m "
	" \x1b[44m║           │2│                                    │+▒▒∞│                 ║\x1b[40m "
	" \x1b[44m║           └─┘                                    └────┘ Transferring... ║\x1b[40m "
	" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");
	send_rom();
	printf("\x1b[22;0H\x1b[40m"
	" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
	" \x1b[44m║ GBA Donut ┌─┐╔══════════════════√══════════════════╗                    ║\x1b[40m "
	" \x1b[44m║           │Θ╪╝                                   ╒═╨─═╕                 ║\x1b[40m "
	" \x1b[44m║           │2│                                    │+▒▒∞│                 ║\x1b[40m "
	" \x1b[44m║           └─┘                                    └────┘        Success! ║\x1b[40m "
	" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");
		sleep(3);
	MODPlay_Pause(&play, false);
}

void genSplash(const char *str, char *out) {
	size_t length = strlen(str);
	int needed;

	// splash should always be 40 characters long
	if (length >= 40) {
		needed = 0;
	} else {
		needed = 40 - length;
	}
	memset(out, ' ', needed);
	out[needed] = '\0';
	strcat(out, str);
}

const char* const splash_messages[SPLASH_COUNT] = {
	[0] = "Also try DS Donut!",
	[1] = "Better than Wii Donut! [citation needed]",
	[2] = "oh man please to help i am not good with",
	[3] = "(\"Doughnut\" if you're british)",
	[4] = "Korbo loves you",
};

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
	bool showmore = false, paused = true;
	char splash[40];

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

	srand(time(NULL));

	genSplash(splash_messages[rand() % SPLASH_COUNT] ?: "FLAGRANT SPLASH ERROR", splash);

	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	puts("\x1b[2;0H");

	// blue background
	//printf("\x1b[44;37m");
	puts("\x1b[40;37m");
	puts("\x1b[2J");

	MODPlay_Init(&play);
	MODPlay_SetMOD(&play,music_mod);
	MODPlay_SetVolume(&play,63,63);
	MODPlay_Start(&play);

	prepare_rom();

	float A = 0.000004, B = 0.000002;
	float i, j;
	int k;
	float z[1760];
	char b[1760];
	int color = 107;
	do {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if (pressed & WPAD_BUTTON_HOME) {
			exit(0);
		} else if (pressed & WPAD_BUTTON_1) {
			sendDonut();
		} else if (pressed & WPAD_BUTTON_2) {
			showmore = !showmore;
		} else if (pressed & WPAD_BUTTON_A) {
			color++;
			color %= 7;
		} else if (pressed & WPAD_BUTTON_B) {
			paused = !paused;
			MODPlay_Pause(&play, paused);
		}
		printf("\x1b[H\x1b[%um", color + 40);
		memset(b,32,1760);
		memset(z,0,7040);
		for (j=0; j < TAU; j += 0.07) {
			for (i=0; i < 6.28; i += 0.02) {
				float c = sin(i);
				float d = cos(j);
				float e = sin(A);
				float f = sin(j);
				float g = cos(A);
				float h = d + 2;
				float D = 1 / (c * h * e + f * g + 5);
				float l = cos(i);
				float m = cos(B);
				float n = sin(B);
				float t = c * h * g - f * e;
				int x = 40 + 30 * D * (l * h * m - t * n);
				int y= 12 + 15 * D * (l * h * n + t * m);
				int o = x + 80 * y;
				int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
				if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
					z[o] = D;
					b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
				}
			}
		}
		for (k = 0; k < 1760; k++) {
			putchar(k % 40 ? b[k] : 8);
			A += 0.00004;
			B += 0.00002;
		}
		usleep(30000);
		if (showmore) {
			printf("\x1b[22;0H\x1b[40m"
			" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
			" \x1b[44m║ A    - Change BG color                                                  ║\x1b[40m "
			" \x1b[44m║ B    - Toggle music                                                     ║\x1b[40m "
			" \x1b[44m║ 1    - Send GBA Donut                                                   ║\x1b[40m "
			" \x1b[44m║ HOME - Exit                                         Press 2 to go back. ║\x1b[40m "
			" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m");
		} else {
			printf("\x1b[22;0H\x1b[40m"
			" \x1b[44m╔═════════════════════════════════════════════════════════════════════════╗\x1b[40m "
			" \x1b[44m║ Korbo's Wii Donut Mod %s   %s ║\x1b[40m "
			" \x1b[44m║ Based on the original donut.c by Andy Sloane <andy@a1k0n.net>           ║\x1b[40m "
			" \x1b[44m║ Ported by emilydaemon <emilydaemon@donut.eu.org>, Modified by Korbo     ║\x1b[40m "
			" \x1b[44m║ Music by Jogeir Liljedahl                        Press 2 for more info. ║\x1b[40m "
			" \x1b[44m╚═════════════════════════════════════════════════════════════════════════╝\x1b[40m", VERSION, splash);
		}
		VIDEO_WaitVSync();
	} while (true);

	return 0;
}
