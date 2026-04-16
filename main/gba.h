#ifndef GBA_H
#define GBA_H

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

//from my tests 50us seems to be the lowest
//safe si transfer delay in between calls
#define SI_TRANS_DELAY 50

extern s8 gba_init();
extern bool is_gba_present();
extern void gba_send();

#endif
