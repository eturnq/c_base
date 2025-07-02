#include "../memory.h"

#include <stdlib.h>
#include <string.h>

static Result raw_heap_alloc(Allocator* allocator, unsigned int length) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0) {
		return res;
	}

	res.data.data = malloc(length);
	if (res.data.data == 0) {
		return res;
	}
	res.data.length = length;
	res.status = ERROR_OK;

	return res;
}

static Result raw_heap_realloc(Allocator* allocator, Slice ptr, unsigned int length) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0 || length == 0) {
		return res;
	}

	res.data.data = realloc(ptr.data, length);
	if (res.data.data == 0) {
		return res;
	}
	res.data.length = length;
	res.status = ERROR_OK;

	return res;
}

static Result raw_heap_free(Allocator* allocator, Slice ptr) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	free(ptr.data);
	ptr.data = 0;
	ptr.length = 0;
	res.status = ERROR_OK;

	return res;
}

static Result raw_heap_freeall(Allocator *allocator) {
	Result res;
	BASE_ERROR_RESULT(res);
	return res;
}

static Result raw_heap_clone(Allocator *allocator, Slice ptr) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	res = ALLOC(allocator, ptr.length);
	if (res.status != ERROR_OK) {
		return res;
	}

	memcpy(res.data.data, ptr.data, ptr.length);
	return res;
}

static Allocator raw_heap_allocator = {
	raw_heap_alloc, raw_heap_realloc,
	raw_heap_free,  raw_heap_freeall,
	raw_heap_clone
};

Allocator *get_raw_heap_allocator(void) {
	return &raw_heap_allocator;
}
