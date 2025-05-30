#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiiuse/wiiuse.h>
#include <aesndlib.h>
#include <gcmodplay.h>
#include <string.h>
#include <math.h>

#include "goombasend.h"

#include "music_mod.h"

#define TAU 6.2831853071795864

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
static MODPlay play;

u8 *resbuf, *cmdbuf;
volatile u32 transval = 0;
volatile u32 resval = 0;

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
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
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");

	// blue background
	//printf("\x1b[44;37m");
	printf("\x1b[40;37m");
	printf("\x1b[2J");

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
	bool senddemo = false;
	for(;;) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) {
			exit(0);
		} else if ( pressed & WPAD_BUTTON_1 )
			senddemo = true;
		printf("\x1b[H");
		memset(b,32,1760);
			memset(z,0,7040);
			for (j=0; j < TAU; j += 0.07) {
				for (i=0; i < TAU; i += 0.02) {
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
					int y = 12 + 15 * D * (l * h * n + t * m);
					int o = x + 80 * y;

					int N = 12 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
					if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
						z[o] = D;
						b[o] = ".^;i+*}j%\\umqkUQM@"[N > 0 ? N : 0];
					}
					if (senddemo) {
						if (!wait_for_gba()) {
							MODPlay_Stop(&play);
							send_rom();
							MODPlay_Start(&play);
							senddemo = false;
						}
					}
				}
			}
		for(k = 0; k < 1760; k++) {
			// printf("\x1b[40;37m");
			printf("\x1b[%um%c", color + 40, (k % 40 ? b[k] : 8));
			A += 0.00004;
			B += 0.00002;
		}
		usleep(30000);
		printf("\x1b[22;0H");
		printf("\x1b[40m \x1b[44m,-------------------------------------------------------------------------,\x1b[40m ");
		printf(" \x1b[44m| Korbo's Wii Donut Mod v2.0                        (Press HOME to quit.) |\x1b[40m ");
		printf(" \x1b[44m| Based on the original donut.c by Andy Sloane <andy@a1k0n.net>           |\x1b[40m ");
		printf(" \x1b[44m| Ported by jornmann <jornmann@duck.com> Modified by Korbo                |\x1b[40m ");
		printf(" \x1b[44m| Music by Jogeir Liljedahl     A - Change BG color   1 - Send GBA Donut  |\x1b[40m ");
		printf(" \x1b[44m`-------------------------------------------------------------------------'\x1b[40m");
		if ( pressed & WPAD_BUTTON_A ) {
			color++;
			color %= 7;
		}
		VIDEO_WaitVSync();
	}

	return 0;
}
