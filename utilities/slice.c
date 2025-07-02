#include "../utilities.h"

#include <stdio.h>
#include <string.h>

int slice_cmp(Slice a, Slice b) {
	unsigned int max_length;
	if (a.length < b.length) {
		max_length = a.length;
	} else {
		max_length = b.length;
	}

	return strncmp(a.data, b.data, max_length);
}

Slice slice_sub(Slice a, int offset, unsigned int length) {
	unsigned int actual_offset;
	static Slice b = { 0, 0 };

	if (a.length == 0 || a.data == 0) {
		return b;
	}

	if (offset < 0) {
		actual_offset = a.length - offset;
	} else {
		actual_offset = offset;
	}

	if (actual_offset > a.length) {
		actual_offset %= a.length;
	}

	if (actual_offset + length > a.length) {
		return b;
	}

	b.length = length;
	b.data = ((uint8_t*)a.data) + actual_offset;
	return b;
}
