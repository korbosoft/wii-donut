#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
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
#include "goombasend.h"
#include "strings.h"
#include "text.h"
#include "flavors.h"
#include "music.h"

#include "music_mod.h"

#define DEFAULT_FIFO_SIZE	(256*1024)

// static GXRModeObj *rmode = NULL;
static void *cxfb = NULL;

u8 *resbuf, *cmdbuf;
volatile u32 transval = 0, resval = 0;

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
			music_pause(!prevPaused);
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

	// Initialize the file... thing. i can't really call it the "file system", can i
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
	float aspect;
	// if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
	// 	aspect = 16.0f/9.0f;
	// } else {
	aspect = 4.0f/3.0f;
	// }

	float donAspect = aspect;

	donAspect *= 77.0f / 44.0f; // effectively halves the width to match the character aspect
	// char boobs[] = " -:=+>|%}Ics1aeCo34wSZkhAE&D$HWQ";

	music_init(title);

	PROXY_3dMode(0.1F, 300.0F, 45, true, false, donAspect);
	GX_SetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM,  GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
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
			print("\e[23;0H" "\e[104;37m" STRING_CONTROLS_BOX "\e[40m");
			// } else if (showFrosting) {

		} else {
			printf("\e[23;0H" "\e[104;37m"
			"╔═══════════════════════════════════════════════════════════════════════════╗"
			"║ \e[4mKorbo's Donut Shop :3 %s   %s\e[0m\e[104;37m "                      "║"
			"║ Inspired by \"Wii Donut\" by emilydaemon <emilydaemon@donut.eu.org>         ║"
			"║ Written, and otherwise created by Korbo Q. Lamp                           ║"
			"║ Default Music is \"Addiction\" by Jogeir Liljedahl  " STRING_CONTROLS   " ║"
			"╚═══════════════════════════════════════════════════════════════════════════╝\e[40m", VERSION, splash);
			// printf("cwd: %s\n", getcwd(NULL, 0));
		}

		if (showFrosting)
			showFrosting--;

		format_info("Flavor: ", frosting[frostingFlavor].name, frostingName);
		printf("\e[0;0H" "%s" "\e[0;0m", (showFrosting != 0) ? frostingName : title);

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
			showFrosting = 50;
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
