#include "file.h"

bool file_exists(const char *path) {
	FILE *f = fopen(path, "rb");
	if (f != NULL) {
		fclose(f);
		return true;
	}
	return false;
}
