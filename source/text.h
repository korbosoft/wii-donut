#ifndef TEXT_H
#define TEXT_H

#include <gccore.h>
#include <string.h>
#include <stdio.h>

extern int calculate_padding(const char *input, size_t resultingLength);
extern int print(const char *str);
extern void format_splash(const char *input, char *output);
extern int rgb_escape(const u8 r, const u8 g, const u8 b, const bool fg);

#endif
