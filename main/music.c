#include <stdlib.h>
#include <ogc/conf.h>
#include <string.h>
#include <grrmod.h>

#include "music.h"

#include "file.h"
#include "text.h"

#include "music_mod.h"

#define FILETYPE_COUNT 16

static bool paused = false;
static u8 *module;

static const char *moduleTypes[FILETYPE_COUNT] = {
	[0] = "669",
	[1] = "amf",
	// [2] = "apun",
	[2] = "dsm",
	[3] = "far",
	[4] = "gdm",
	[5] = "imf",
	[6] = "it",
	[7] = "med",
	[8] = "mod",
	[9] = "mtm",
	[10] = "okt",
	[11] = "s3m",
	[12] = "stm",
	[13] = "stx",
	[14] = "ult",
	// [15] = "uni",
	[15] = "xm",
};

static bool is_title_empty(const char *title) {
	if (title[0] == '\0') return true;

	const int length = strlen(title) - 1;
	int i;
	bool isAllSpaces = false;
	for (i = 0; i < length; i++) {
		isAllSpaces |= (title[i] == ' ');
	}
	return isAllSpaces;
}

static bool music_attempt(const char *type) {
	char tmp[11] = "music.";
	strcat(tmp, type);
	return file_exists(tmp);
}

void music_init(char *title_display) {
	char tmp[11] = "music.";
	u8 i;

	for (i = 0; i < FILETYPE_COUNT; i++) {
		if (music_attempt(moduleTypes[i])) {
			strcat(tmp, moduleTypes[i]);
			break;
		}
	}

	FILE *f;
	f = file_open(tmp, "rb");

	#ifndef GC
	GRRMOD_Init(CONF_GetSoundMode()); // check for mono
	#else
	GRRMOD_Init(SYS_GetSoundMode()); // check for mono
	#endif
	if (f) {
		fseek(f, 0, SEEK_END);
		long module_size = ftell(f);
		rewind(f);

		module = (u8 *)malloc(module_size);

		fread(module, 1, module_size, f);

		GRRMOD_SetMOD(module,module_size);
		file_close(f);
	} else {
		GRRMOD_SetMOD(music_mod, music_mod_size);
		strcat(tmp, "mod"); // built-in song should always have a title but whatever
	}

	char *title = GRRMOD_GetSongTitle();
	format_info("Song: ", is_title_empty(title) ? tmp : title, title_display);
	// format_info("Song: ", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", title_display);

	GRRMOD_Start();
}

void music_free(void) {
	if (module)
		free(module);
	GRRMOD_End();
}

void music_pause(const bool pause) {
	if (pause != paused) {
		GRRMOD_Pause();
		paused = !paused;
	}
}
