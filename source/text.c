#include <string.h>
#include <stdio.h>

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
