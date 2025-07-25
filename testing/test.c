#include "test.h"
#include "heap_test.h"
#include "linear_alloc_test.h"
#include "arraylist_test.h"
#include "queue_test.h"
#include "slice_test.h"
#include "stack_test.h"

TestResult *always_passes(TestResult* result) {
	INIT_RESULT(result, "[always_passes]");
	result->status = TEST_PASS;
	return result;
}

#define TEST_COUNT 29
Test tests[TEST_COUNT] = {
	always_passes,
	slice_compare,
	slice_sub_test,
	slice_copy_test,
	heap_allocation,
	heap_reallocation,
	heap_freeall_should_fail,
	heap_clone,
	stack_init_deinit,
	stack_push_pop,
	queue_init_deinit,
	queue_push_pop,
	array_list_init_deinit,
	array_list_push,
	array_list_pop,
	array_list_get,
	array_list_index_of,
	array_list_remove,
	array_list_insert,
	array_list_swap,
	array_list_replace,
	heap_slice_split,
	basic_linear_alloc_init_deinit,
	basic_linear_alloc_alloc_free,
	basic_linear_alloc_freeall,
	basic_linear_alloc_clone,
	linear_alloc_init_deinit,
	linear_alloc_alloc_free,
	linear_alloc_freeall,
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
