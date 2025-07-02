#include "stack_test.h"
#include "../memory.h"

TestResult *stack_init_deinit(TestResult *result) {
	INIT_RESULT(result, "[stack_init_deinit]");

	Allocator* raw_heap = get_raw_heap_allocator();
	Result stack_result = new_stack_collection(raw_heap, sizeof(int), 1);
	if (stack_result.status != ERROR_OK || IS_NULL_SLICE(stack_result.data)) {
		MSG_PRINT(result, " Unable to instantiate new stack collection");
		return result;
	}

	StackCollection stack = *((StackCollection*)stack_result.data.data);

	stack_result = deinit_stack_collection(&stack);
	if (stack_result.status != ERROR_OK) {
		MSG_PRINT(result, " Unabel to deinitialize stack collection");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *stack_push_pop(TestResult *result) {
	INIT_RESULT(result, "[stack_push_pop]");

	Allocator* raw_heap = get_raw_heap_allocator();
	Result stack_result = new_stack_collection(raw_heap, sizeof(int), 16);
	StackCollection stack = *((StackCollection*)stack_result.data.data);

	int int1 = 1;
	int int2 = 2;
	Slice int1_s = { sizeof(int), &int1 };
	Slice int2_s = { sizeof(int), &int2 };

	stack_result = LINEAR_PUSH(&stack, int1_s);
	if (stack_result.status != ERROR_OK) {
		MSG_PRINT(result, " Unable to push first value to stack collection");
		deinit_stack_collection(&stack);
		return result;
	}

	stack_result = LINEAR_PUSH(&stack, int2_s);
	if (stack_result.status != ERROR_OK) {
		MSG_PRINT(result, " Unable to push second value to stack collection");
		deinit_stack_collection(&stack);
		return result;
	}

	stack_result = LINEAR_POP(&stack);
	if (stack_result.status != ERROR_OK || slice_cmp(int2_s, stack_result.data) != 0) {
		MSG_PRINT(result, " Popped wrong value (should be 2");
		deinit_stack_collection(&stack);
		return result;
	}
	stack_result = LINEAR_POP(&stack);
	if (stack_result.status != ERROR_OK || slice_cmp(int1_s, stack_result.data) != 0) {
		MSG_PRINT(result, " Popped wrong value (should be 1");
		deinit_stack_collection(&stack);
		return result;
	}

	deinit_stack_collection(&stack);
	result->status = TEST_PASS;
	return result;
}
