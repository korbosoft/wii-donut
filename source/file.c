#include "file.h"

int fatInitResult;

void file_init(void) {
	fatInitResult = fatInitDefault();
}

FILE *file_open(const char *filename, const char *mode) {
	if (fatInitResult) {
		return fopen(filename, mode);
	}
	return NULL;
}

int file_close(FILE *stream) {
	if (fatInitResult) {
		return fclose(stream);
	}
	return -1;
}

bool file_exists(const char *filename) {
	FILE *f = file_open(filename, "rb");
	if (f != NULL) {
		file_close(f);
		return true;
	}
	return false;
}
