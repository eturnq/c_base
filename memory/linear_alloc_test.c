#include "linear_alloc_test.h"
#include "../memory.h"

TestResult *linear_alloc_init_deinit(TestResult* result) {
    Allocator *heap;
    LinearAllocator *linear;
    Result res;
    INIT_RESULT(result, "[linear_alloc_init_deinit] ");

    heap = get_raw_heap_allocator();
    res = new_linear_allocator(heap, 1024);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to create the linear allocator with 1024 bytes");
        return result;
    }
    if (res.data.length != sizeof(LinearAllocator)) {
        sprintf(
            result->message + strlen(result->message),
            "Heap allocator return incorrect size: %u should be 1024",
            res.data.length
        );
        FREE(heap, res.data);
        return result;
    }

    linear = (LinearAllocator*)res.data.data;
    if (linear->position != 0) {
        sprintf(
            result->message + strlen(result->message),
            "LinearAllocator has invalid position: %u should be 0",
            linear->position
        );
        deinit_linear_allocator(linear);
        return result;
    }

    if (linear->buffer.length != 1024) {
        sprintf(
            result->message + strlen(result->message),
            "LinearAllocator has invalid buffer size: %u should be 1024",
            linear->buffer.length
        );
        deinit_linear_allocator(linear);
        return result;
    }

    if (linear->free_blocks.item_count != 0) {
        sprintf(
            result->message + strlen(result->message),
            "LinearAllocator has invalid free blocks: %u should be 0",
            linear->free_blocks.item_count
        );
        deinit_linear_allocator(linear);
        return result;
    }

    res = deinit_linear_allocator(linear);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to deinitialize LinearAllocator");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *linear_alloc_alloc_free(TestResult* result) {
    INIT_RESULT(result, "[linear_alloc_alloc_free]");

    // TODO: finish this test
    return result;
}
