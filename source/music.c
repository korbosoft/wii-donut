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

bool music_attempt(const char *type) {
	char tmp[11] = "music.";
	strcat(tmp, type);
	return file_exists(tmp);
}

void music_init(char *title_display) {
	char tmp[20] = "music.";
	u8 i;

	for (i = 0; i < 18; i++) {
		if (music_attempt(moduleTypes[i])) {
			strcat(tmp, moduleTypes[i]);
			break;
		}
	}


	FILE *f = fopen(tmp, "rb");

	if (f != NULL) {
		if (strcmp(moduleTypes[i], "s3m") == 0) { // certain s3ms supposedly break GRRMOD in stereo
			GRRMOD_Init(false);
		} else {
			GRRMOD_Init(true);
		}

		fseek(f, 0, SEEK_END);
		long module_size = ftell(f);
		rewind(f);

		module = (u8 *)malloc(module_size);

		fread(module, 1, module_size, f);

		GRRMOD_SetMOD(module,module_size);
	} else {
		GRRMOD_Init(true); // music_mod is a mod file, (obviously) so no checks needed
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
