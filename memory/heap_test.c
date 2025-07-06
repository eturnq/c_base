#include "heap_test.h"

#include "../memory.h"

TestResult *heap_allocation(TestResult *result) {
	INIT_RESULT(result, "[heap_allocation]");

	Allocator* raw_heap = get_raw_heap_allocator();

	Result alloc_res = ALLOC(raw_heap, 5);
	if (alloc_res.status != ERROR_OK || IS_NULL_SLICE(alloc_res.data) || alloc_res.data.length != 5) {
		MSG_PRINT(result, " Raw allocation failed");
		return result;
	}

	Result alloc_free = FREE(raw_heap, alloc_res.data);
	if (alloc_free.status != ERROR_OK || !IS_NULL_SLICE(alloc_free.data)) {
		sprintf(result->message + strlen(result->message), " Raw free failed");
		MSG_PRINT(result, " Raw free failed");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *heap_reallocation(TestResult *result) {
	INIT_RESULT(result, "[heap_reallocation]");

	Allocator* raw_heap = get_raw_heap_allocator();

	Result alloc_res = ALLOC(raw_heap, 5);
	Result realloc_res = REALLOC(raw_heap, alloc_res.data, 10);

	if (realloc_res.status != ERROR_OK) {
		MSG_PRINT(result, " Raw reallocation returned an error");
		FREE(raw_heap, alloc_res.data);
		return result;
	}

	if (IS_NULL_SLICE(realloc_res.data) || realloc_res.data.length != 10) {
		sprintf((char*)result->message + strlen(result->message), " Raw reallocation returned invalid slice: length %u", realloc_res.data.length);
		FREE(raw_heap, alloc_res.data);
		if (realloc_res.data.length != 0) {
			FREE(raw_heap, realloc_res.data);
		}
		return result;
	}

	FREE(raw_heap, realloc_res.data);
	result->status = TEST_PASS;

	return result;
}

TestResult *heap_freeall_should_fail(TestResult *result) {
	INIT_RESULT(result, "[heap_freeall_should_fail]");

	Allocator* raw_heap = get_raw_heap_allocator();
	Result raw_heap_free = FREEALL(raw_heap);
	if (raw_heap_free.status != ERROR_ERR || !IS_NULL_SLICE(raw_heap_free.data)) {
		MSG_PRINT(result, " freeall succeeds on raw heap allocator");
		return result;
	}
	Result raw_heap_alloc = ALLOC(raw_heap, 1024);
	raw_heap_free = FREEALL(raw_heap);

	if (raw_heap_free.status != ERROR_ERR || !IS_NULL_SLICE(raw_heap_free.data)) {
		MSG_PRINT(result, " freeall succeeds on raw heap allocator after allocation");
		FREE(raw_heap, raw_heap_alloc.data);
		return result;
	}

	FREE(raw_heap, raw_heap_alloc.data);
	result->status = TEST_PASS;
	return result;
}

TestResult *heap_clone(TestResult *result) {
    INIT_RESULT(result, "[heap_clone]");

    Allocator* raw_heap = get_raw_heap_allocator();
    Slice a = { "This is a test", 14 };

    Result res = CLONE(raw_heap, a);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to clone the Slice");
        return result;
    }

    if (slice_cmp(a, res.data) != 0) {
        FREE(raw_heap, res.data);
        MSG_PRINT(result, " Slices are not equivalent");
        return result;
    }

    FREE(raw_heap, res.data);
    result->status = TEST_PASS;
    return result;
}
