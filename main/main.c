#include <math.h>
#include <unistd.h>
#include "donut.h"
#include "file.h"
#include "flavors.h"
#include "font.h"
#include "goombasend.h"
#include "input.h"
#include "music.h"
#include "strings.h"
#include "text.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

u8 *resbuf, *cmdbuf;
volatile u32 transval = 0;
volatile u32 resval = 0;

static bool paused = true;
static u8 frostingFlavor = 0;

static void send_donut(void) {
	bool prev_paused = paused;
	music_pause(true);

	print("\e[23;0H" "\e[0;0m\e[40;37m" "\e[104;37m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \e[4mGBA Donut\e[0;0m\e[104;37m ┌─┐ Connect your GBA to controller port     " STRING_CANCEL " ║"
	"║           │\xfb│ 2 with a GBA link cable.             ╒═──═╕                 ║"
	"║           │2│                                      │+░░∞│                 ║"
	"║           └─┘                                      └────┘      Waiting... ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\e[0m");

	while (wait_for_gba()) {
		input_scan();
		input_down(0, 0);
		if ((wiiPressed & WPAD_BUTTON_PLUS) | (GCPressed & PAD_BUTTON_Y)) {
			music_pause(!prev_paused);
			return;
		}
	}

	print("\e[23;0H" "\e[104;37m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \e[4mGBA Donut\e[0m\e[104;37m ┌─┐╔═══════════════════════════════════════╗                    ║"
	"║           │\xfc╪╝                                     ╒═╨─═╕                 ║"
	"║           │2│                                      │+▒▒∞│                 ║"
	"║           └─┘                                      └────┘ Transferring... ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\e[0m");

	send_rom();

	print("\e[23;0H" "\e[104;37m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \e[4mGBA Donut\e[0m\e[104;37m ┌─┐╔══════════════════√════════════════════╗                    ║"
	"║           │\xfc╪╝                                     ╒═╨─═╕                 ║"
	"║           │2│                                      │+▓▓∞│                 ║"
	"║           └─┘                                      └────┘        Success! ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\e[0m");

	sleep(3);
	music_pause(!prev_paused);
}

#define SPLASH_COUNT 6

static const char *splashMessages[SPLASH_COUNT] = {
	[0] = "Also try DS Donut!",
	[1] = "Better than Wii Donut! [citation needed]",
	[2] = "oh man please to help i am not good with c",
	[3] = "(\"Doughnut\" if you're british)",
	[4] = "Korbo loves you <3",
	[5] = "Did you know you can change the music?",
};

//---------------------------------------------------------------------------------
int main() {
	bool showControls = false;
	char splash[43], title[82], frostingName[82], doughName[82];
	//---------------------------------------------------------------------------------
	// Initialise the video system
	VIDEO_Init();

	// Initialize all controller input functions
	input_init();

	// Initialise the audio subsystem
	AESND_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	CON_Init(xfb,20,20,rmode->fbWidth-20,rmode->xfbHeight-20,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Clear the framebuffer
	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

	// Set custom font
	consoleSetFont(NULL, &(ConsoleFont){(u8*)donut_font_8x16, 0, 256});

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
	u8 showFrosting = 0;
	do {
		input_scan();
		input_down(0, 0);
		if ((wiiPressed & WPAD_BUTTON_1) | (GCPressed & PAD_TRIGGER_Z)) {
			send_donut();
		} else if ((wiiPressed & WPAD_BUTTON_2) | (GCPressed & PAD_BUTTON_B)) {
			showControls = !showControls;
		} else if ((wiiPressed & WPAD_BUTTON_MINUS) | (GCPressed & PAD_BUTTON_X)) {
			// bgColor++;
			// bgColor %= 7;
		} else if ((wiiPressed & WPAD_BUTTON_PLUS) | (GCPressed & PAD_BUTTON_Y)) {
			frostingFlavor++;
			frostingFlavor %= FROSTING_FLAVORS;
			showFrosting = 50;
		} else if ((wiiPressed & WPAD_BUTTON_A) | (GCPressed & PAD_BUTTON_A)) {
			music_pause(paused);
			paused = !paused;
		}
		render_frame(A, B, frosting[frostingFlavor]);
		A = fmod(A + THETA_SPACING, PI_TIMES_2);
		B = fmod(B + PHI_SPACING, PI_TIMES_2);
		format_info("Frosting: ", frosting[frostingFlavor].name, frostingName);
		if (showControls) {
			print("\e[23;0H" "\e[104;37m" STRING_CONTROLS_BOX "\e[40m");
		} else {
			printf("\e[23;0H" "\e[104;37m"
			"╔═══════════════════════════════════════════════════════════════════════════╗"
			"║ \e[4mKorbo's Wii Donut Mod %s   %s\e[0m\e[104;37m "                      "║"
			"║ Based on the original donut.c by Andy Sloane <andy@a1k0n.net>             ║"
			"║ Ported by emilydaemon <emilydaemon@donut.eu.org>, Modified by Korbo       ║"
			"║ Default Music by Jogeir Liljedahl                 " STRING_CONTROLS     " ║"
			"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m", VERSION, splash);
		// printf("cwd: %s\n", getcwd(NULL, 0));
		}
		printf("\e[0;0H" "%s" "\e[0;0m", (showFrosting != 0) ? frostingName : title);
		if (showFrosting)
			showFrosting--;
		VIDEO_WaitVSync();
	} while (!(wiiPressed & WPAD_BUTTON_HOME) & !(GCPressed & PAD_BUTTON_START));
	music_free();
	return 0;
}
