#ifndef MUSIC_H
#define MUSIC_H

#include <aesndlib.h>

extern int music_init(char *title_display);
extern void music_pause(const bool pause);
extern void music_free(void);

#endif
