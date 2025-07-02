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

#define INIT_RESULT(result, name) \
result->status = TEST_FAIL; \
sprintf(result->message, name)

#define MSG_PRINT(result, msg) sprintf(result->message + strlen(result->message), msg)

TestResult *always_passes(TestResult* result) {
	INIT_RESULT(result, "[always_passes]");
	result->status = TEST_PASS;
	return result;
}

TestResult *slice_compare(TestResult *result) {
	INIT_RESULT(result, "[slice_compare]");

	int int1 = 1;
	int int2 = 2;
	Slice s1 = { sizeof(int), &int1 };
	Slice s2 = { sizeof(int), &int2 };

	if (slice_cmp(s1, s1) != 0) {
		MSG_PRINT(result, " slice_cmp should return 0");
		return result;
	}

	if (slice_cmp(s1, s2) != -1) {
		MSG_PRINT(result, " slice_cmp should return -1");
		return result;
	}

	if (slice_cmp(s2, s1) != 1) {
		MSG_PRINT(result, " slice_cmp should return 1");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *slice_sub_test(TestResult *result) {
	Slice s1, s2;
	char test_data[20];
	INIT_RESULT(result, "[slice_sub]");

	sprintf(test_data, "This is a test");
	s1.data = test_data;
	s1.length = 14;

	s2 = slice_sub(s1, 5, 9);

	if (s2.length != 9) {
		MSG_PRINT(result, " s2 has the wrong length");
		return result;
	}

	if (strncmp("is a test", s2.data, s2.length) != 0) {
		MSG_PRINT(result, " s2 has the wrong data");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

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
    Slice a = { 14, "This is a test" };

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

TestResult *queue_init_deinit(TestResult *result) {
	INIT_RESULT(result, "[queue_init_deinit]");

	Allocator *raw_heap = get_raw_heap_allocator();
	Result queue_res = new_queue_collection(raw_heap, sizeof(int), 2);
	if (queue_res.status != ERROR_OK || queue_res.data.length != sizeof(QueueCollection) || queue_res.data.data == 0) {
		MSG_PRINT(result, " Unable to instantiate new queue collection");
		return result;
	}

	QueueCollection queue = *((QueueCollection*)queue_res.data.data);
	queue_res = deinit_queue_collection(&queue);
	if (queue_res.status != ERROR_OK || !IS_NULL_SLICE(queue_res.data)) {
		MSG_PRINT(result, " Unable to deinitialize queue collection");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *queue_push_pop(TestResult *result) {
	INIT_RESULT(result, "[queue_push_pop]");

	Allocator *raw_heap = get_raw_heap_allocator();
	Result queue_res = new_queue_collection(raw_heap, sizeof(int), 4 );
	QueueCollection queue = *((QueueCollection*)queue_res.data.data);

	int int1 = 1;
	int int2 = 2;
	Slice int1_s = { sizeof(int), &int1 };
	Slice int2_s = { sizeof(int), &int2 };

	queue_res = LINEAR_PUSH(&queue, int1_s);
	if (queue_res.status != ERROR_OK) {
		MSG_PRINT(result, " Unable to push first value");
		deinit_queue_collection(&queue);
		return result;
	}

	queue_res = LINEAR_PUSH(&queue, int2_s);
	if (queue_res.status != ERROR_OK) {
		MSG_PRINT(result, " Unable to push second value");
		deinit_queue_collection(&queue);
		return result;
	}

	queue_res = LINEAR_POP(&queue);
	if (queue_res.status != ERROR_OK || slice_cmp(queue_res.data, int1_s) != 0) {
		MSG_PRINT(result, " Unable to pop value 1");
		deinit_queue_collection(&queue);
		return result;
	}

	queue_res = LINEAR_POP(&queue);
	if (queue_res.status != ERROR_OK || slice_cmp(queue_res.data, int2_s) != 0) {
		MSG_PRINT(result, " Unable to pop value 2");
		deinit_queue_collection(&queue);
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *array_list_init_deinit(TestResult *result) {
    INIT_RESULT(result, "[array_list_init_deinit]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to create new ArrayList");
        return result;
    }

    ArrayList al = *((ArrayList*)res.data.data);

    res = deinit_array_list(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to deinit ArrayList");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_push_pop(TestResult *result) {
    INIT_RESULT(result, "[array_list_push_pop]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    ArrayList al = *((ArrayList*)res.data.data);
    int int1 = 1;
    int int2 = 2;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };

    res = LINEAR_PUSH(&al, s1);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push first value");
        deinit_array_list(&al);
        return result;
    }

    res = LINEAR_PUSH(&al, s2);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push second value");
        deinit_array_list(&al);
        return result;
    }

    res = LINEAR_POP(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to pop second value");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(res.data, s2) != 0) {
        MSG_PRINT(result, " Popped the wrong value for value 2");
        deinit_array_list(&al);
        return result;
    }

    res = LINEAR_POP(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to pop first value");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(res.data, s1) != 0) {
        MSG_PRINT(result, " Popped the wrong value for value 1");
        deinit_array_list(&al);
        return result;
    }

    if (al.item_count != 0) {
        MSG_PRINT(result, " Item count should be 0 after all items popped");
        deinit_array_list(&al);
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_get(TestResult *result) {
    INIT_RESULT(result, "[array_list_get]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    ArrayList al = RESULT_UNWRAP(res, ArrayList);
    int int1 = 1;
    int int2 = 2;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };

    LINEAR_PUSH(&al, s1);
    LINEAR_PUSH(&al, s2);

    res = INDEXING_GET(&al, 0);
    Slice temp = res.data;

    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get from index 0");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(temp, s1) != 0) {
        MSG_PRINT(result, " Index 0 returned the wrong value");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_GET(&al, 1);
    temp = res.data;

    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get from index 1");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(temp, s2) != 0) {
        MSG_PRINT(result, " Index 1 returned the wrong value");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_GET(&al, 2);
    if (res.status == ERROR_OK) {
        MSG_PRINT(result, " Get from out-of-bounds returns ERROR_OK");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_index_of(TestResult *result) {
    INIT_RESULT(result, "[array_list_index_of]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    ArrayList al = RESULT_UNWRAP(res, ArrayList);

    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };
    Slice s3 = { sizeof(int), &int3 };

    LINEAR_PUSH(&al, s1);
    LINEAR_PUSH(&al, s2);

    res = INDEXING_INDEX_OF(&al, s1);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get index of s1");
        deinit_array_list(&al);
        return result;
    }
    if (RESULT_UNWRAP(res, unsigned int) != 0) {
        MSG_PRINT(result, " index_of returned the wrong index for s1");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_INDEX_OF(&al, s2);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get index of s2");
        deinit_array_list(&al);
        return result;
    }
    if (RESULT_UNWRAP(res, unsigned int) != 1) {
        MSG_PRINT(result, " index_of returned the wrong index for s2");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_INDEX_OF(&al, s3);
    if (res.status == ERROR_OK) {
        MSG_PRINT(result, " index_of did not return an error when given a value not in the collection");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;

    return result;
}

TestResult *array_list_remove(TestResult *result) {
    INIT_RESULT(result, "[array_list_remove]");

    Allocator *heap = get_raw_heap_allocator();
    ArrayList al = RESULT_UNWRAP(new_array_list(heap, sizeof(int), 16), ArrayList);

    Result res = INDEXING_REMOVE(&al, 0);
    if (res.status == ERROR_OK) {
        MSG_PRINT(result, " remove returned a valid result when given an invalid index");
        deinit_array_list(&al);
        return result;
    }

    int int1 = 1;
    Slice s1 = { sizeof(int), &int1 };
    LINEAR_PUSH(&al, s1);
    res = INDEXING_REMOVE(&al, 0);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " remove return an error when given a valid index");
        deinit_array_list(&al);
        return result;
    }

    if (al.item_count != 0) {
        MSG_PRINT(result, " item count should be zero after item removed");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;
    return result;
}

#define TEST_COUNT 16
Test tests[TEST_COUNT] = {
	always_passes,
	slice_compare,
	slice_sub_test,
	heap_allocation,
	heap_reallocation,
	heap_freeall_should_fail,
	heap_clone,
	stack_init_deinit,
	stack_push_pop,
	queue_init_deinit,
	queue_push_pop,
	array_list_init_deinit,
	array_list_push_pop,
	array_list_get,
	array_list_index_of,
	array_list_remove
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
