#include "text.h"

#include <stdio.h>
#include <string.h>

static void generate_padding(const char *input, const size_t resultingLength, char *output) {
	const size_t strLength = strlen(input);
	size_t paddingNeeded;

	if (strLength >= resultingLength) {
		paddingNeeded = 0;
	} else {
		paddingNeeded = resultingLength - strLength;
	}

	if (paddingNeeded != 0)
		memset(output, ' ', paddingNeeded);
	output[paddingNeeded] = '\0';
}

void format_info(const char *prefix, const char *input, char output[78]) {
	char padding[82];
	generate_padding(input, 76 - strlen(prefix), padding);

	snprintf(output, 81, "%s\e[4m%s%s", padding, prefix, input);
}

void format_splash(const char *input, char *output) {
	char padding[43];
	generate_padding(input, 42, padding);
	snprintf(output, 43, "%s%s", padding, input);
}

int print(const char *str) {
	return fwrite(str, strlen(str), 1, stdout);
}

inline char* u82Str(char* buf, u8 n) {
	if (n >= 100) {
		*buf++ = (n / 100) + '0';
		n %= 100;
		*buf++ = (n / 10) + '0';
		*buf++ = (n % 10) + '0';
	} else if (n >= 10) {
		*buf++ = (n / 10) + '0';
		*buf++ = (n % 10) + '0';
	} else {
		*buf++ = n + '0';
	}
	return buf;
}
