#ifndef INPUT_H
#define INPUT_H

#ifndef GC
#include <wiiuse/wpad.h>
#else
#include <ogc/pad.h>

#define WPAD_BUTTON_2				0x0001
#define WPAD_BUTTON_1				0x0002
#define WPAD_BUTTON_B				0x0004
#define WPAD_BUTTON_A				0x0008
#define WPAD_BUTTON_MINUS			0x0010
#define WPAD_BUTTON_HOME			0x0080
#define WPAD_BUTTON_LEFT			0x0100
#define WPAD_BUTTON_RIGHT			0x0200
#define WPAD_BUTTON_DOWN			0x0400
#define WPAD_BUTTON_UP				0x0800
#define WPAD_BUTTON_PLUS			0x1000

// #define WPAD_NUNCHUK_BUTTON_Z		(0x0001<<16)
// #define WPAD_NUNCHUK_BUTTON_C		(0x0002<<16)
//
// #define WPAD_CLASSIC_BUTTON_UP		(0x0001u<<16)
// #define WPAD_CLASSIC_BUTTON_LEFT	(0x0002u<<16)
// #define WPAD_CLASSIC_BUTTON_ZR		(0x0004u<<16)
// #define WPAD_CLASSIC_BUTTON_X		(0x0008u<<16)
// #define WPAD_CLASSIC_BUTTON_A		(0x0010u<<16)
// #define WPAD_CLASSIC_BUTTON_Y		(0x0020u<<16)
// #define WPAD_CLASSIC_BUTTON_B		(0x0040u<<16)
// #define WPAD_CLASSIC_BUTTON_ZL		(0x0200u<<16)
// #define WPAD_CLASSIC_BUTTON_PLUS	(0x0400u<<16)
// #define WPAD_CLASSIC_BUTTON_HOME	(0x0800u<<16)
// #define WPAD_CLASSIC_BUTTON_MINUS	(0x1000u<<16)
// #define WPAD_CLASSIC_BUTTON_FULL_L	(0x2000u<<16)
// #define WPAD_CLASSIC_BUTTON_DOWN	(0x4000u<<16)
// #define WPAD_CLASSIC_BUTTON_RIGHT	(0x8000u<<16)
#endif

extern u32 wiiPressed;
extern u16 GCPressed;

extern void input_init(void);

extern void input_scan(void);

extern void input_down(u8 wii, u8 gamecube);

#endif
