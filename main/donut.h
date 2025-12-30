#ifndef DONUT_H
#define DONUT_H

#include "flavors.h"

#define PI_TIMES_2 6.2831853071795865

#define SCREEN_WIDTH 77
#define SCREEN_HEIGHT 22
#define SCREEN_SIZE SCREEN_WIDTH * SCREEN_HEIGHT

#define THETA_SPACING 0.07
#define PHI_SPACING 0.02

extern void render_frame(float A, float B, Donut flavor);

#endif
