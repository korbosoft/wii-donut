#ifndef DONUT_H
#define DONUT_H

#include "flavors.h"

#define DONUT_WIDTH 77
#define DONUT_HEIGHT 22
#define DONUT_FOV 45

extern void donut_init(void);
extern void donut_exit(void);
extern void render_frame(float A, float B, Donut flavor);

#endif
