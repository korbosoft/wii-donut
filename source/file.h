#ifndef FILE_H
#define FILE_H

#include <fat.h>
#include <stdio.h>

extern bool file_exists(const char *path);
extern int populate_playlist(char playlist[256][50]);

#endif
