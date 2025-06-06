#include <stdlib.h>
#include <string.h>

#include "music.h"

#include "file.h"
#include "text.h"

#include "music_mod.h"

static bool paused = 0;
static u8 *module;


void format_title(const char *input, char *output) {
	int paddingNeeded;
	char prefix[75];

	strcpy(prefix, "Song: ");
	// title text should always be 20 characters long
	paddingNeeded = calculate_padding(input, 69);
	memset(output, ' ', paddingNeeded);
	output[paddingNeeded] = '\0';
	strcat(prefix, input);
	strcat(output, prefix);
}

void music_load(char *title_display, const char *path) {
	char *tmp = "music/";
	strcat(tmp, path);
	FILE *f = fopen(tmp, "rb");

	if (f != NULL) {
		fseek(f, 0, SEEK_END);
		long module_size = ftell(f);
		rewind(f);

		// allocate memory to contain the whole file:
		module = (u8 *)malloc(module_size);

		// copy the file into the buffer:
		fread(module, 1, module_size, f);

		GRRMOD_SetMOD(module,module_size);
	} else {
		GRRMOD_SetMOD(music_mod, music_mod_size);
	}

	format_title(GRRMOD_GetSongTitle(), title_display);

	fclose(f);
	GRRMOD_Start();
}

void music_free(void) {
	free(module);
}

void music_pause(const bool pause) {
	if (pause != paused) {
		GRRMOD_Pause();
		paused = !paused;
	}
}
