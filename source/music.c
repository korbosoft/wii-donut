#include <stdlib.h>
#include <string.h>

#include "music.h"

#include "file.h"
#include "text.h"

#include "music_mod.h"

static bool paused = false;
static u8 *module;

const char *const moduleTypes[18] = {
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

bool is_title_empty(const char *title) {
	if (title[0] == '\0') return true;

	const int length = strlen(title) - 1;
	int i;
	bool isAllSpaces = false;
	for (i = 0; i < length; i++) {
		isAllSpaces |= (title[i] == ' ');
	}
	return isAllSpaces;
}

void format_title(const char *input, char *output) {
	int paddingNeeded;
	char prefix[82];

	strcpy(prefix, "\e[4mSong: ");

	paddingNeeded = calculate_padding(input, 71);
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
	char tmp[11] = "music.";
	u8 i;
	FILE *f;

	for (i = 0; i < 18; i++) {
		if (music_attempt(moduleTypes[i])) {
			strcat(tmp, moduleTypes[i]);
			break;
		}
	}

	f = file_open(tmp, "rb");

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
		file_close(f);
	} else {
		GRRMOD_Init(true); // music_mod is a mod file, (obviously) so no checks needed
		GRRMOD_SetMOD(music_mod, music_mod_size);
		strcat(tmp, "mod"); // built-in song should always have a title but whatever
	}

	char *title = GRRMOD_GetSongTitle();
	format_title(is_title_empty(title) ? tmp : title, title_display);

	GRRMOD_Start();
}

void music_free(void) {
	free(module);
	GRRMOD_End();
}

void music_pause(const bool pause) {
	if (pause != paused) {
		GRRMOD_Pause();
		paused = !paused;
	}
}
