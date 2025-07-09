#include "../memory.h"
#include "../utilities.h"

#include <stdlib.h>
#include <string.h>

// This function assumes that FREE might fail and destroy data, but not a valid reference
Result standard_realloc(Allocator *allocator, Slice ptr, unsigned int size) {
	Result res;
	Slice new_mem;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.data == 0 || ptr.length == 0 || size == 0) {
		return res;
	}

	res = ALLOC(allocator, size);
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != size) {
		FREE(allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}
	new_mem = res.data;	

	res = slice_copy(new_mem, ptr);
	if (res.status != ERROR_OK) {
		FREE(allocator, new_mem);
		return res;
	}

	res = FREE(allocator, ptr);
	if (res.status != ERROR_OK) {
		slice_copy(ptr, new_mem);
		FREE(allocator, new_mem);
		BASE_ERROR_RESULT(res);
		return res;
	}

	res.status = ERROR_OK;
	res.data = new_mem;
	return res;
}

Result standard_clone(Allocator *allocator, Slice ptr) {
	Result res;
	Slice new_mem;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	res = ALLOC(allocator, ptr.length);
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != ptr.length) {
		FREE(allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}
	new_mem = res.data;

	res = slice_copy(new_mem, ptr);
	if (res.status != ERROR_OK) {
		FREE(allocator, new_mem);
		return res;
	}

	return res;
}

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
	SET_NULL_SLICE(ptr);
	res.status = ERROR_OK;

	return res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static Result raw_heap_freeall(Allocator *allocator) {
	Result res;
	BASE_ERROR_RESULT(res);
	return res;
}
#pragma GCC diagnostic pop

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
