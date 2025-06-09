#include <stdlib.h>
#include <string.h>

#include "music.h"

#include "file.h"
#include "text.h"

#include "music_mod.h"

static bool paused = 0;
static u8 *module;

const char* const moduleTypes[18] = {
	[0] = "669",
	[1] = "amf",
	[2] = "apun",
	[3] = "dsm",
	[4] = "far",
	[5] = "gdm",
	[6] = "imf",
	[7] = "it",
	[8] = "med",
	[9] = "mod",
	[10] = "mtm",
	[11] = "okt",
	[12] = "s3m",
	[13] = "stm",
	[14] = "stx",
	[15] = "ult",
	[16] = "uni",
	[17] = "xm"
};

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

inline bool music_attempt(const char *type) {
	char tmp[11] = "music.";
	strcat(tmp, type);
	return file_exists(tmp);
}

void music_load(char *title_display) {
	char tmp[11];
	u8 i;

	for (i = 0; i < 18; i++) {
		if (music_attempt(moduleTypes[i])) {
			strcpy(tmp, moduleTypes[i]);
		}
	}

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
