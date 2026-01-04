#include "input.h"

u32 wiiPressed;
u16 GCPressed;
s8 HWButton = -1;

static void ResetPressed(u32 thesevariables, void *arenotused) {
	HWButton = SYS_HOTRESET;
}

#ifndef GC
static void PowerPressed() {
	HWButton = SYS_POWEROFF;
}

static void WiimotePowerPressed(s32 chan) {
	if (chan == 0) PowerPressed();
}
#endif

void input_init(void) {
	#ifndef GC
	WPAD_Init();
	WPAD_SetPowerButtonCallback(WiimotePowerPressed);
	SYS_SetPowerCallback(PowerPressed);
	#endif
	PAD_Init();
	SYS_SetResetCallback(ResetPressed);
}

void input_scan(void) {
	#ifndef GC
	WPAD_ScanPads();
	#endif
	PAD_ScanPads();
}
void input_down(u8 wii, u8 gamecube) {
	#ifndef GC
	wiiPressed = WPAD_ButtonsDown(wii);
	#endif
	GCPressed = PAD_ButtonsDown(gamecube);
}
