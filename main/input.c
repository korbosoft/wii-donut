#include "input.h"

u32 wiiPressed;
u16 GCPressed;

void input_init(void) {
	#ifndef GC
	WPAD_Init();
	#endif
	PAD_Init();
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
