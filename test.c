#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "utilities.h"

enum test_status {
	TEST_PASS,
	TEST_FAIL
};

#define TEST_MESSAGE_SIZE 100
typedef struct {
	enum test_status status;
	char message[TEST_MESSAGE_SIZE];
} TestResult;

typedef TestResult *(*Test)(TestResult*);

TestResult *always_passes(TestResult* result) {
	result->status = TEST_PASS;
	strcpy(result->message, "[always_passes]");
	return result;
}

TestResult *heap_allocation(TestResult *result) {
	result->status = TEST_FAIL;
	strcpy(result->message, "[heap_allocation]");

	Allocator* raw_heap = get_raw_heap_allocator();

	Result alloc_res = ALLOC(raw_heap, 5);
	if (alloc_res.status != ERROR_OK || IS_NULL_SLICE(alloc_res.data) || alloc_res.data.length != 5) {
		strcpy((char*)result->message + strlen(result->message), " Raw allocation failed");
		return result;
	}

	Result alloc_free = FREE(raw_heap, alloc_res.data);
	if (alloc_free.status != ERROR_OK || !IS_NULL_SLICE(alloc_free.data)) {
		strcpy((char*)result->message + strlen(result->message), " Raw free failed");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *heap_reallocation(TestResult *result) {
	result->status = TEST_FAIL;
	strcpy(result->message, "[heap_reallocation]");

	Allocator* raw_heap = get_raw_heap_allocator();

	Result alloc_res = ALLOC(raw_heap, 5);
	Result realloc_res = REALLOC(raw_heap, alloc_res.data, 10);

	if (realloc_res.status != ERROR_OK) {
		strcpy((char*)result->message + strlen(result->message), " Raw reallocation returned an error");
		FREE(raw_heap, alloc_res.data);
		return result;
	}

	if (IS_NULL_SLICE(realloc_res.data) || realloc_res.data.length != 10) {
		sprintf((char*)result->message + strlen(result->message), " Raw realoocation returned invalid slice: length %u", realloc_res.data.length);
		FREE(raw_heap, alloc_res.data);
		if (realloc_res.data.length != 0) {
			FREE(raw_heap, realloc_res.data);
		}
		return result;
	}

	FREE(raw_heap, alloc_res.data);
	FREE(raw_heap, realloc_res.data);
	result->status = TEST_PASS;

	return result;
}

TestResult *heap_freeall_should_fail(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[heap_freeall_should_fail]");

	Allocator* raw_heap = get_raw_heap_allocator();
	Result raw_heap_free = FREEALL(raw_heap);
	if (raw_heap_free.status != ERROR_ERR || !IS_NULL_SLICE(raw_heap_free.data)) {
		sprintf(result->message + strlen(result->message), " freeall succeeds on raw heap allocator");
		return result;
	}
	Result raw_heap_alloc = ALLOC(raw_heap, 1024);
	raw_heap_free = FREEALL(raw_heap);

	if (raw_heap_free.status != ERROR_ERR || !IS_NULL_SLICE(raw_heap_free.data)) {
		sprintf(result->message + strlen(result->message), " freeall succeeds on raw heap allocator after allocation");
		FREE(raw_heap, raw_heap_alloc.data);
		return result;
	}
	
	FREE(raw_heap, raw_heap_alloc.data);
	result->status = TEST_PASS;
	return result;
}

#define TEST_COUNT 4
Test tests[TEST_COUNT] = {
	always_passes,
	heap_allocation,
	heap_reallocation,
	heap_freeall_should_fail
};

int main() {
	printf("Testing base objects and routines...\n");

	TestResult result;
	char out_buffer[128];
	for (int index = 0; index < TEST_COUNT; index++) {
		if (tests[index](&result)->status == TEST_FAIL) {
			strcpy(out_buffer, "[FAILURE] ");
			strncpy(out_buffer + strlen(out_buffer), result.message, TEST_MESSAGE_SIZE);
			fprintf(stderr, "%s\n", out_buffer);
			return -1;
		}
		strcpy(out_buffer, "[SUCCESS] ");
		strncpy(out_buffer + strlen(out_buffer), result.message, TEST_MESSAGE_SIZE);
		printf("%s\n", out_buffer);
	}
	printf("All tests passed!\n");

	return 0;
}
