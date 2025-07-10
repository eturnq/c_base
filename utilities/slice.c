#include "../utilities.h"
#include <string.h>

int slice_cmp(Slice a, Slice b) {
	unsigned int max_length;
	#ifdef DEBUG_SET
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-variable"
	char *a_str = (char *)a.data;
	char *b_str = (char *)b.data;
	#pragma GCC diagnostic pop
	#endif

	if (a.length == 0 || a.data == 0) {
	    return -1;
	}

	if (b.length == 0 || b.data == 0) {
	    return 1;
	}

	if (a.length == b.length && a.data == b.data) {
		return 0;
	}

	if (a.length < b.length) {
		max_length = a.length;
	} else {
		max_length = b.length;
	}

	return memcmp(a.data, b.data, max_length);
}

Slice slice_sub(Slice a, int offset, unsigned int length) {
	unsigned int actual_offset;
	Slice b = { 0, 0 };

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
	b.data = (void*)((uint8_t*)a.data + actual_offset);
	return b;
}

Result slice_copy(Slice a, Slice b) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (b.length == 0 || b.data == 0 || a.length < b.length || a.data == 0) {
        return res;
    }

    memcpy(a.data, b.data, b.length);

    res.status = ERROR_OK;
    res.data.length = b.length;
    res.data.data = a.data;

    return res;
}
