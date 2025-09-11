#ifndef MUSIC_H
#define MUSIC_H

#include <stdlib.h>
#include <aesndlib.h>
#include <grrmod.h>

extern void music_init(char *title_display);
extern void music_pause(const bool pause);
extern void music_free(void);

#endif
