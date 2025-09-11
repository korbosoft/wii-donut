#include "text.h"

int calculate_padding(const char *input, size_t resultingLength) {
	const size_t strLength = strlen(input);
	int paddingNeeded;

	if (strLength >= resultingLength) {
		paddingNeeded = 0;
	} else {
		paddingNeeded = resultingLength - strLength;
	}
	return paddingNeeded;
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

void format_name(const char *input, char *output) {
	int paddingNeeded;
	char prefix[82];

	strcpy(prefix, "\e[4mFlavor: ");

	paddingNeeded = calculate_padding(input, 69);
	memset(output, ' ', paddingNeeded);
	output[paddingNeeded] = '\0';
	strcat(prefix, input);
	strcat(output, prefix);
}

void format_splash(const char *input, char *output) {
	int paddingNeeded;

	paddingNeeded = calculate_padding(input, 42);
	memset(output, ' ', paddingNeeded);
	output[paddingNeeded] = '\0';
	strcat(output, input);
}

int print(const char *str) {
	return fwrite(str, strlen(str), 1, stdout);
}

int rgb_escape(const u8 r, const u8 g, const u8 b, const bool fg) {
	if ((r > 255) || (g > 255) || (b > 255))
		return -1;
	printf("\e[%i;2;%i;%i;%im", fg ? 38 : 48, r, g, b);
	return 0;
}
