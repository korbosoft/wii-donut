#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

int print(const char *str) {
	return fwrite(str, strlen(str), 1, stdout);
}

static int compare(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

void alph_sort(const char* str[], int n) {
	qsort(str, n, sizeof(const char*), compare);
}
