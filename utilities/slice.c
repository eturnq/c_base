#include "../utilities.h"

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
