#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <aesndlib.h>
#include <gcmodplay.h>
#include <grrlib.h>

#include "colors.h"
#include "donut.h"
#include "file.h"
#include "grrproxy.h"
#include "input.h"
#include "font.h"
#include "gba.h"
#include "strings.h"
#include "text.h"
#include "flavors.h"
#include "music.h"

#include "music_mod.h"

#define DEFAULT_FIFO_SIZE	(256*1024)

// static GXRModeObj *rmode = NULL;
static void *cxfb = NULL;

static bool paused = true;
static u8 frostingFlavor = 0;

#define SPLASH_COUNT 7

static const char *splashMessages[SPLASH_COUNT] = {
	[0] = "Also try DS Donut!",
	[1] = "Also try Lily Skate!",
	[2] = "Better than Wii Donut!",
	[3] = "oh man please to help i am not good with c",
	[4] = "(\"Doughnut\" if you're british)",
	[5] = "Korbo loves you <3",
	[6] = "Did you know you can change the music?",
};

static void send_donut(void) {
	bool prevPaused = paused;
	music_pause(true);

	print("\x1b[23;0H" "\x1b[0;0m\x1b[40;37m" "\x1b[104;37m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \x1b[4mGBA Donut\x1b[0;0m\x1b[104;37m ┌─┐ Connect your GBA to controller port     " STRING_CANCEL " ║"
	"║           │\xfb│ 4 with a GBA link cable.             ╒═──═╕                 ║"
	"║           │4│                                      │+░░∞│                 ║"
	"║           └─┘                                      └────┘      Waiting... ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m");

	while (!is_gba_present()) {
		input_scan();
		input_down(0, 0);
		if ((wiiPressed & WPAD_BUTTON_PLUS) | (GCPressed & PAD_BUTTON_Y)) {
			music_pause(!prevPaused);
			return;
		}
	}

	print("\x1b[23;0H" "\x1b[104;37m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \x1b[4mGBA Donut\x1b[0m\x1b[104;37m ┌─┐╔═══════════════════════════════════════╗                    ║"
	"║           │\xfc╪╝                                     ╒═╨─═╕                 ║"
	"║           │4│                                      │+▒▒∞│                 ║"
	"║           └─┘                                      └────┘ Transferring... ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m");

	gba_send();

	print("\x1b[23;0H" "\x1b[104;37m"
	"╔═══════════════════════════════════════════════════════════════════════════╗"
	"║ \x1b[4mGBA Donut\x1b[0m\x1b[104;37m ┌─┐╔══════════════════√════════════════════╗                    ║"
	"║           │\xfc╪╝                                     ╒═╨─═╕                 ║"
	"║           │4│                                      │+▓▓∞│                 ║"
	"║           └─┘                                      └────┘        Success! ║"
	"╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m");

	sleep(3);
	music_pause(!prevPaused);
}

int main(int argc,char **argv) {
	char splash[43], title[82], frostingName[82], doughName[82];
	bool showControls = false;
	guVector lpos = {2.0f, 2.0f, 2.0f};
	GXLightObj lobj;

	GRRLIB_Init();

	input_init();
	// WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC);

	gba_init();

	// Initialize the file... thing. I can't really call it the "file system", can I?
	file_init();

	// Allocate memory for the display in the uncached region
	cxfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	VIDEO_ClearFrameBuffer(rmode, cxfb, COLOR_BLACK);

	// Initialise the console, required for printf
	console_init(cxfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	consoleSetFont(NULL, &(ConsoleFont){(u8*)donut_font_8x16, 0, 256});
	//SYS_STDIO_Report(true);
	VIDEO_SetNextFramebuffer(cxfb);

	// setup our projection matrix
	float aspect = 4.0f/3.0f;
	// if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
	// 	aspect = 16.0f/9.0f;
	// } else {
	// 	aspect = 4.0f/3.0f;
	// }

	float donAspect = aspect;

	donAspect *= 77.0f / 44.0f; // effectively halves the width to match the character aspect
	// char boobs[] = " -:=+>|%}Ics1aeCo34wSZkhAE&D$HWQ";

	music_init(title);

	PROXY_3dMode(0.1F, 300.0F, 45, true, true, donAspect);
	GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
	PROXY_Camera3dSettings(0.0f,0.0f,0.0f, 0,1,0, 0,0,0);

	donut_init();

	float A = 1, B = 1;

	if (rand() % 49) {
		format_splash(splashMessages[rand() % SPLASH_COUNT] ?: "FLAGRANT SPLASH ERROR", splash);
	} else {
		format_splash("This splash has a 1/50 chance of appearing", splash);
	}

	u8 showFrosting = 0;
	GX_SetCullMode(GX_CULL_FRONT);
	while(SYS_MainLoop()) {
		GX_SetNumChans(1);
		guVecMultiply(view, &lpos, &lpos);

		GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
		GX_InitLightColor(&lobj, LC_WHITE);
		GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_MEDIUM);
		GX_LoadLightObj(&lobj,GX_LIGHT0);

		GX_SetChanAmbColor(GX_COLOR0A0, LC_DARKER);
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);

		render_frame(A, B, frosting[frostingFlavor]);

		input_scan();
		input_down(0, 0);

		if (showControls) {
			print("\x1b[23H" "\x1b[104;37m" STRING_CONTROLS_BOX "\x1b[40m");
			// } else if (showFrosting) {

		} else {
			printf("\x1b[23H" "\x1b[104;37m"
			"╔═══════════════════════════════════════════════════════════════════════════╗"
			"║ \x1b[4mKorbo's Donut Shop :3 %s   %s\x1b[0m\x1b[104;37m "                "║"
			"║ Inspired by \"Wii Donut\" by emilydaemon <emilydaemon@donut.eu.org>         ║"
			"║ Written, and otherwise created by Korbo Q. Lamp                           ║"
			"║ Default Music is \"Addiction\" by Jogeir Liljedahl  " STRING_CONTROLS   " ║"
			"╚═══════════════════════════════════════════════════════════════════════════╝\x1b[40m", VERSION, splash);
			// printf("cwd: %s\n", getcwd(NULL, 0));
		}

		if (showFrosting)
			showFrosting--;

		format_info("Flavor: ", frosting[frostingFlavor].name, frostingName);
		printf("\x1b[H" "%s" "\x1b[0;0m", (showFrosting != 0) ? frostingName : title);

		VIDEO_Flush();
		VIDEO_WaitVSync();
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
			showFrosting = 100;
		} else if ((wiiPressed & WPAD_BUTTON_A) | (GCPressed & PAD_BUTTON_A)) {
			music_pause(paused);
			paused = !paused;
		} else if ((wiiPressed & WPAD_BUTTON_HOME) || (GCPressed & PAD_BUTTON_START))
			break;

		A += 0.035f;
		B += 0.01f;
	}

	GRRLIB_2dMode();
	donut_exit();
	GRRLIB_Exit();
	music_free();
	exit(0);
}
