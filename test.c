#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "utilities/queue.h"
#include "utilities/stack.h"

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

TestResult *slice_compare(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[slice_compare]");

	int int1 = 1;
	int int2 = 2;
	Slice s1 = { sizeof(int), &int1 };
	Slice s2 = { sizeof(int), &int2 };

	if (slice_cmp(s1, s1) != 0) {
		sprintf(result->message + strlen(result->message), " slice_cmp should return 0");
		return result;
	}

	if (slice_cmp(s1, s2) != -1) {
		sprintf(result->message + strlen(result->message), " slice_cmp should return -1");
		return result;
	}

	if (slice_cmp(s2, s1) != 1) {
		sprintf(result->message + strlen(result->message), " slice_cmp should return 1");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *heap_allocation(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[heap_allocation]");

	Allocator* raw_heap = get_raw_heap_allocator();

	Result alloc_res = ALLOC(raw_heap, 5);
	if (alloc_res.status != ERROR_OK || IS_NULL_SLICE(alloc_res.data) || alloc_res.data.length != 5) {
		sprintf(result->message + strlen(result->message), " Raw allocation failed");
		return result;
	}

	Result alloc_free = FREE(raw_heap, alloc_res.data);
	if (alloc_free.status != ERROR_OK || !IS_NULL_SLICE(alloc_free.data)) {
		sprintf(result->message + strlen(result->message), " Raw free failed");
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

TestResult *stack_init_deinit(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[stack_init_deinit]");

	Allocator* raw_heap = get_raw_heap_allocator();
	Result stack_result = new_stack_collection(raw_heap, sizeof(int), 1);
	if (stack_result.status != ERROR_OK || IS_NULL_SLICE(stack_result.data)) {
		sprintf(result->message + strlen(result->message), " Unable to instantiate new stack collection");
		return result;
	}

	StackCollection stack = *((StackCollection*)stack_result.data.data);

	stack_result = deinit_stack_collection(&stack);
	if (stack_result.status != ERROR_OK) {
		sprintf(result->message + strlen(result->message), " Unable to deinitialize stack collection");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *stack_push_pop(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[stack_push_pop]");

	Allocator* raw_heap = get_raw_heap_allocator();
	Result stack_result = new_stack_collection(raw_heap, sizeof(int), 16);
	StackCollection stack = *((StackCollection*)stack_result.data.data);

	int int1 = 1;
	int int2 = 2;
	Slice int1_s = { sizeof(int), &int1 };
	Slice int2_s = { sizeof(int), &int2 };

	stack_result = LINEAR_PUSH(&stack, int1_s);
	if (stack_result.status != ERROR_OK) {
		sprintf(result->message + strlen(result->message), "Unable to push first value to stack collection");
		deinit_stack_collection(&stack);
		return result;
	}

	stack_result = LINEAR_PUSH(&stack, int2_s);
	if (stack_result.status != ERROR_OK) {
		sprintf(result->message + strlen(result->message), "Unable to push second value to stack collection");
		deinit_stack_collection(&stack);
		return result;
	}

	stack_result = LINEAR_POP(&stack);
	if (stack_result.status != ERROR_OK || slice_cmp(int2_s, stack_result.data) != 0) {
		sprintf(result->message + strlen(result->message), "Popped wrong value (should be 2)");
		deinit_stack_collection(&stack);
		return result;
	}
	stack_result = LINEAR_POP(&stack);
	if (stack_result.status != ERROR_OK || slice_cmp(int1_s, stack_result.data) != 0) {
		sprintf(result->message + strlen(result->message), "Popped wrong value (should be 1)");
		deinit_stack_collection(&stack);
		return result;
	}

	deinit_stack_collection(&stack);
	result->status = TEST_PASS;
	return result;
}

TestResult *queue_init_deinit(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[queue_init_deinit]");

	Allocator *raw_heap = get_raw_heap_allocator();
	Result queue_res = new_queue_collection(raw_heap, sizeof(int), 2);
	if (queue_res.status != ERROR_OK || queue_res.data.length != sizeof(QueueCollection) || queue_res.data.data == 0) {
		sprintf(result->message + strlen(result->message), " Unable to instatiate new queue collection");
		return result;
	}

	QueueCollection queue = *((QueueCollection*)queue_res.data.data);
	queue_res = deinit_queue_collection(&queue);
	if (queue_res.status != ERROR_OK || !IS_NULL_SLICE(queue_res.data)) {
		sprintf(result->message + strlen(result->message), " Unable to deinitialize queue collection");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *queue_push_pop(TestResult *result) {
	result->status = TEST_FAIL;
	sprintf(result->message, "[queue_push_pop]");

	Allocator *raw_heap = get_raw_heap_allocator();
	Result queue_res = new_queue_collection(raw_heap, sizeof(int), 2 );
	QueueCollection queue = *((QueueCollection*)queue_res.data.data);

	int int1 = 1;
	int int2 = 2;
	Slice int1_s = { sizeof(int), &int1 };
	Slice int2_s = { sizeof(int), &int2 };
	
	queue_res = LINEAR_PUSH(&queue, int1_s);
	if (queue_res.status != ERROR_OK) {
		sprintf(result->message + strlen(result->message), " Unable to push first value");
		deinit_queue_collection(&queue);
		return result;
	}

	queue_res = LINEAR_PUSH(&queue, int2_s);
	if (queue_res.status != ERROR_OK) {
		sprintf(result->message + strlen(result->message), " Unable to push second value");
		deinit_queue_collection(&queue);
		return result;
	}

	queue_res = LINEAR_POP(&queue);
	if (queue_res.status != ERROR_OK || slice_cmp(queue_res.data, int2_s) != 0) {
		sprintf(result->message + strlen(result->message), " Unable to pop value 2");
		deinit_queue_collection(&queue);
		return result;
	}

	queue_res = LINEAR_POP(&queue);
	if (queue_res.status != ERROR_OK || slice_cmp(queue_res.data, int1_s) != 0) {
		sprintf(result->message + strlen(result->message), " Unable to pop value 1");
		deinit_queue_collection(&queue);
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

#define TEST_COUNT 9
Test tests[TEST_COUNT] = {
	always_passes,
	slice_compare,
	heap_allocation,
	heap_reallocation,
	heap_freeall_should_fail,
	stack_init_deinit,
	stack_push_pop,
	queue_init_deinit,
	queue_push_pop
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
	}
	printf("All tests passed!\n");

	return 0;
}
