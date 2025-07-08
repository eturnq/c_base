#include "linear_alloc_test.h"
#include "../memory.h"

TestResult *basic_linear_alloc_init_deinit(TestResult* result) {
    Result res;
    Allocator *heap;
    BASE_ERROR_RESULT(res);
    INIT_RESULT(result, "[basic_linear_alloc_init_deinit] ");

    heap = get_raw_heap_allocator();
    res = new_basic_linear_allocator(heap, 256);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to initialize allocator");
        return result;
    }

    res = deinit_basic_linear_allocator(res.data.data);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to deinitialize allocator");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *basic_linear_alloc_alloc_free(TestResult* result) {
    Allocator *heap;
    BasicLinearAllocator *linear;
    Result res;
    INIT_RESULT(result, "[basic_linear_alloc_alloc_free] ");

    heap = get_raw_heap_allocator();
    res = new_basic_linear_allocator(heap, 256);
    linear = (BasicLinearAllocator*) res.data.data;

    res = ALLOC((Allocator*)linear, 8);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to allocate 8 bytes");
        deinit_basic_linear_allocator(linear);
        return result;
    }
    if (res.data.length != 8) {
        sprintf(
            result->message + strlen(result->message),
            "Result slice has invalid length: %u should be %u",
            res.data.length, 8
        );
        deinit_basic_linear_allocator(linear);
        return result;
    }

    res = FREE((Allocator*)linear, res.data);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to free 8 bytes");
        deinit_basic_linear_allocator(linear);
        return result;
    }

    deinit_basic_linear_allocator(linear);
    result->status = TEST_PASS;

    return result;
}

TestResult *basic_linear_alloc_freeall(TestResult* result) {
    Allocator *heap;
    BasicLinearAllocator *linear;
    Result res;
    INIT_RESULT(result, "[basic_linear_alloc_freeall] ");

    heap = get_raw_heap_allocator();
    res = new_basic_linear_allocator(heap, 256);
    linear = (BasicLinearAllocator*) res.data.data;
    ALLOC((Allocator*)linear, 8);
    ALLOC((Allocator*)linear, 8);
    res = FREEALL((Allocator*)linear);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to freeall");
        deinit_basic_linear_allocator(linear);
        return result;
    }
    if (slice_cmp(linear->buffer, linear->current) != 0) {
        MSG_PRINT(result, "Current not reset to buffer");
        deinit_basic_linear_allocator(linear);
        return result;
    }

    deinit_basic_linear_allocator(linear);
    result->status = TEST_PASS;
    return result;
}

TestResult *basic_linear_alloc_clone(TestResult* result) {
    Allocator *heap;
    BasicLinearAllocator *linear;
    Result res;
    Slice s;
    INIT_RESULT(result, "[basic_linear_alloc_clone] ");

    heap = get_raw_heap_allocator();
    res = new_basic_linear_allocator(heap, 256);
    linear = (BasicLinearAllocator*) res.data.data;
    s.data = "This is a test";
    s.length = strlen(s.data);

    res = CLONE((Allocator*)linear, s);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, "Unable to perform clone");
        deinit_basic_linear_allocator(linear);
        return result;
    }
    if (res.data.length != s.length) {
        sprintf(
            result->message + strlen(result->message),
            "Returned slice has invalid length: %u should be %u",
            res.data.length, s.length
        );
        deinit_basic_linear_allocator(linear);
        return result;
    }
    if (slice_cmp(res.data, s) != 0) {
        MSG_PRINT(result, "Returned slice is invalid");
        deinit_basic_linear_allocator(linear);
        return result;
    }

    deinit_basic_linear_allocator(linear);
    result->status = TEST_PASS;
    return result;
}
