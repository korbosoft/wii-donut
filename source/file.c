#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "file.h"

bool file_exists(const char *path) {
	FILE *f = fopen(path, "rb");
	if (f != NULL) {
		fclose(f);
		return true;
	}
	return false;
}

int populate_playlist(char playlist[256][50]) {
	int i;
	DIR *dir;
	struct dirent *ent;
	struct stat statbuf;

	dir=opendir("./music");

	if (!dir)
		return 0;

	for (i=0; ((ent=readdir(dir))!=NULL) & (i<=50); ) {
		stat(ent->d_name,&statbuf);
		if(!(S_ISDIR(statbuf.st_mode)))
			strcpy(playlist[i], ent->d_name);
		i++;
	}
	closedir(dir);
	return i;
}
