#include "../globals.h"
#include "../memory.h"
#include "../utilities.h"

function Result basic_linear_alloc(Allocator* allocator, unsigned int size) {
    Result res;
    BasicLinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || size == 0) {
        return res;
    }

    linear = (BasicLinearAllocator*) allocator;
    if (size > linear->current.length) {
        return res;
    }

    res.data.length = size;
    res.data = slice_sub(linear->current, 0, size);
    linear->current.length -= size;
    linear->current.data = (void*)((uint8_t*)linear->current.data + size);

    res.status = ERROR_OK;
    return res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
function Result basic_linear_realloc(Allocator* allocator, Slice ptr, unsigned int size) {
    // TODO: implement more result statuses
    Result res;
    res.status = ERROR_OK;
    res.data.length = 0;
    res.data.data = 0;

    return res;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
function Result basic_linear_free(Allocator* allocator, Slice ptr) {
    // TODO: implement more result statuses
    Result res;
    res.status = ERROR_OK;
    res.data.length = 0;
    res.data.data = 0;

    return res;
}
#pragma GCC diagnosic pop

function Result basic_linear_freeall(Allocator *allocator) {
    Result res;
    BasicLinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0) {
        return res;
    }

    linear = (BasicLinearAllocator*) allocator;
    linear->current = linear->buffer;

    res.status = ERROR_OK;

    return res;
}

function Result basic_linear_clone(Allocator *allocator, Slice ptr) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
        return res;
    }

    res = ALLOC(allocator, ptr.length);
    if (res.status != ERROR_OK) {
        return res;
    }
    memcpy(res.data.data, ptr.data, ptr.length);

    return res;
}

Result new_basic_linear_allocator(Allocator* allocator, unsigned int max_size) {
    Result res;
    BasicLinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || max_size == 0) {
        return res;
    }

    res = ALLOC(allocator, sizeof(BasicLinearAllocator));
    if (res.status != ERROR_OK) {
        return res;
    }
    if (res.data.length != sizeof(BasicLinearAllocator)) {
        FREE(allocator, res.data);
        BASE_ERROR_RESULT(res);
        return res;
    }
    linear = (BasicLinearAllocator*) res.data.data;

    res = ALLOC(allocator, max_size);
    if (res.status != ERROR_OK) {
        return res;
    }
    if (res.data.length != max_size) {
        FREE(allocator, res.data);
        res.data.data = linear;
        res.data.length = sizeof(BasicLinearAllocator);
        FREE(allocator, res.data);
        BASE_ERROR_RESULT(res);
        return res;
    }
    linear->buffer = res.data;
    linear->current = res.data;
    linear->inside_methods = allocator;

    linear->outside_methods.alloc = basic_linear_alloc;
    linear->outside_methods.realloc = basic_linear_realloc;
    linear->outside_methods.free = basic_linear_free;
    linear->outside_methods.freeall = basic_linear_freeall;
    linear->outside_methods.clone = basic_linear_clone;

    res.data.data = linear;
    res.data.length = sizeof(BasicLinearAllocator);
    return res;
}

Result deinit_basic_linear_allocator(BasicLinearAllocator* linear) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (linear == 0) {
        return res;
    }

    res = FREE(linear->inside_methods, linear->buffer);
    if (res.status != ERROR_OK) {
        return res;
    }

    res.data.data = linear;
    res.data.length = sizeof(BasicLinearAllocator);
    res = FREE(linear->inside_methods, res.data);
    if (res.status != ERROR_OK) {
        return res;
    }

    res.status = ERROR_OK;
    return res;
}

Result init_linear_allocator(Allocator* allocator, unsigned int max_size) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || max_size == 0) {
        return res;
    }

    // TODO: finish this function
    return res;
}

Result deinit_linear_allocator(Allocator* self) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (self == 0) {
        return res;
    }

    // TODO: finish this function
    return res;
}
