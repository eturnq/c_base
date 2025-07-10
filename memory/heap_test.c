#include "heap_test.h"

#include "../globals.h"
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

function Result check_word(TestResult *result, char *word, unsigned int index, ArrayList *split) {
    	Slice check;
	Result res;
	BASE_ERROR_RESULT(res);

	check.length = strlen(word);
	check.data = (void*) word;
	res = INDEXING_GET(split, index);
	if (res.status != ERROR_OK) {
		sprintf(
			result->message + strlen(result->message),
			"Unable to get index %d from split",
			0
		);
		deinit_array_list(split);
		res.data.length = sizeof(TestResult);
		res.data.data = result;
		res.status = ERROR_ERR;
		return res;
	}
	if (slice_cmp(*(Slice *)res.data.data, check) != 0) {
		sprintf(
    			result->message + strlen(result->message), 
    			"Word %u is incorrect",
    			index
		);
		res.data.length = sizeof(TestResult);
		res.data.data = result;
		res.status = ERROR_ERR;
		return res;
	}

	res.status = ERROR_OK;
	return res;
}

TestResult *heap_slice_split(TestResult *result) {
    	ArrayList *split;
    	Result res;
    	Slice check;
	INIT_RESULT(result, "[heap_slice_split] ");

	Allocator* raw_heap = get_raw_heap_allocator();
	/*res = init_linear_allocator(raw_heap, 4096);
	if (res.status != ERROR_OK) {
		MSG_PRINT(result, "Unable to create linear allocator");
		return result;
	}*/
	//Allocator *linear = (Allocator *) res.data.data;
	Slice a = { "This is a test", 14 };
	Slice b = { " ", 1 };

	res = SLICE_SPLIT(raw_heap, a, b);
	if (res.status != ERROR_OK) {
		MSG_PRINT(result, "Unable to split the slice");
		return result;
	}
	split = (ArrayList *) res.data.data;

	if (split->item_count != 4) {
		sprintf(
			result->message + strlen(result->message),
			"Split has incorrect length: %u should be %u",
			split->item_count, 4
    		);
		deinit_array_list(split);
		return result;
	}

	// Check first word
	res = check_word(result, "This", 0, split);
	if (res.status != ERROR_OK) {
    		deinit_array_list(split);
		return (TestResult *) res.data.data;
	}
	// Check second word
	res = check_word(result, "is", 1, split);
	if (res.status != ERROR_OK) {
    		deinit_array_list(split);
		return (TestResult *) res.data.data;
	}
	// Check third word
	res = check_word(result, "a", 2, split);
	if (res.status != ERROR_OK) {
    		deinit_array_list(split);
		return (TestResult *) res.data.data;
	}
	// Check fourth word
	res = check_word(result, "test", 3, split);
	if (res.status != ERROR_OK) {
    		deinit_array_list(split);
		return (TestResult *) res.data.data;
	}

	res = deinit_array_list_items(split);
	if (res.status != ERROR_OK) {
		MSG_PRINT(result, "Unable to deinit arraylist items");
		deinit_array_list(split);
		res.data.data = split;
		res.data.length = sizeof(ArrayList);
		FREE(raw_heap, res.data);
		return result;
	}
	deinit_array_list(split);
	res.data.data = split;
	res.data.length = sizeof(ArrayList);
	FREE(raw_heap, res.data);
	//deinit_linear_allocator(linear);

	result->status = TEST_PASS;
	return result;
}

