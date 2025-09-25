#ifndef TEXT_H
#define TEXT_H

#include <gccore.h>

extern int print(const char *str);
extern void format_splash(const char *input, char *output);
extern int rgb_escape(const u8 r, const u8 g, const u8 b, const bool fg);
extern void format_title(const char *input, char *output);
extern void format_name(const char *input, char *output);


#endif
