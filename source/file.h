#ifndef FILE_H
#define FILE_H

#include <fat.h>
#include <stdio.h>

extern void file_init(void);
extern FILE *file_open(const char *filename, const char *mode);
extern int file_close(FILE *stream);
extern bool file_exists(const char *filename);

#endif
