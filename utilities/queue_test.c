#include "queue_test.h"
#include "../memory.h"
#include "../utilities.h"

TestResult *queue_init_deinit(TestResult *result) {
	INIT_RESULT(result, "[queue_init_deinit]");

	Allocator *raw_heap = get_raw_heap_allocator();
	QueueCollection queue;
	Result queue_res = new_queue_collection(&queue, raw_heap, sizeof(int), 2);
	if (queue_res.status != ERROR_OK || queue_res.data.length != sizeof(QueueCollection) || queue_res.data.data == 0) {
		MSG_PRINT(result, " Unable to instantiate new queue collection");
		return result;
	}

	queue_res = deinit_queue_collection(&queue);
	if (queue_res.status != ERROR_OK || IS_NULL_SLICE(queue_res.data)) {
		MSG_PRINT(result, " Unable to deinitialize queue collection");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *queue_push_pop(TestResult *result) {
	INIT_RESULT(result, "[queue_push_pop]");

	Allocator *raw_heap = get_raw_heap_allocator();
	QueueCollection queue;
	Result queue_res = new_queue_collection(&queue, raw_heap, sizeof(int), 4 );

	int int1 = 1;
	int int2 = 2;
	Slice int1_s = { &int1, sizeof(int) };
	Slice int2_s = { &int2, sizeof(int) };

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
