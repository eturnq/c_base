#include "../globals.h"
#include "../memory.h"
#include "../utilities.h"

function Result linear_alloc(Allocator* allocator, unsigned int size) {
    Result res;
    LinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || size == 0) {
        return res;
    }
    linear = (LinearAllocator*) allocator;

    Iterator free_iter = INDEXING_ITER(&linear->free_blocks);
    Slice *current_block;
    do {
        res = ITER_NEXT(&free_iter);
        current_block = res.data.data;
    } while (res.status == ERROR_OK && current_block->length < size);

    if (res.status == ERROR_OK) {
        res.data.length = size;
        res.data.data = current_block->data;

        current_block->length -= size;
        current_block->data = (void*)(((uint8_t*)current_block->data) + size);
        return res;
    }

    if (linear->position + size > linear->buffer.length) {
        return res;
    }

    res.status = ERROR_OK;
    res.data.length = size;
    res.data.data = (void*) ((uint8_t*)linear->buffer.data + linear->position);
    linear->position += size;

    return res;
}

function Result linear_realloc(Allocator *allocator, Slice ptr, unsigned int size) {
    Result res;
    //LinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    // TODO: finish this function
    return res;
}

function Result linear_free(Allocator *allocator, Slice ptr) {
    Result res;
    LinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
        return res;
    }

    linear = (LinearAllocator*) allocator;
    uint8_t* ptr_cmp = (uint8_t*)ptr.data;
    uint8_t* linear_cmp = (uint8_t*)linear->buffer.data;
    if (ptr_cmp < linear_cmp || ptr_cmp > (linear_cmp + linear->position)) {
        return res;
    }

    res = LINEAR_PUSH(&linear->free_blocks, ptr);
    if (res.status != ERROR_OK) {
        return res;
    }

    res.data.length = 0;
    res.data.data = 0;

    return res;
}

function Result linear_freeall(Allocator *allocator) {
    Result res;
    LinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0) {
        return res;
    }

    linear = (LinearAllocator*) allocator;

    res = FREE(linear->inside_methods, linear->buffer);
    if (res.status != ERROR_OK) {
        return res;
    }

    linear->position = 0;
    linear->free_blocks.item_count = 0;

    res.status = ERROR_OK;
    return res;
}

function Result linear_clone(Allocator* allocator, Slice ptr) {
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

Result new_linear_allocator(Allocator* allocator, unsigned int max_size) {
    Result res;
    LinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || max_size == 0) {
        return res;
    }

    res = ALLOC(allocator, sizeof(LinearAllocator));
    if (res.status != ERROR_OK) {
        return res;
    }
    if (res.data.length != sizeof(LinearAllocator)) {
        FREE(allocator, res.data);
        BASE_ERROR_RESULT(res);
        return res;
    }
    linear = (LinearAllocator*)res.data.data;

    res = ALLOC(allocator, max_size);
    if (res.status != ERROR_OK) {
        return res;
    }
    if (res.data.length != max_size) {
        FREE(allocator, res.data);
        BASE_ERROR_RESULT(res);
        return res;
    }
    linear->buffer = res.data;

    res = new_array_list(&linear->free_blocks, allocator, sizeof(Slice), 16);
    if (res.status != ERROR_OK) {
        FREE(allocator, linear->buffer);
        Slice s = { &linear, sizeof(LinearAllocator) };
        FREE(allocator, s);
        return res;
    }

    linear->position = 0;
    linear->inside_methods = allocator;
    linear->outside_methods.alloc = linear_alloc;
    linear->outside_methods.realloc = linear_realloc;
    linear->outside_methods.free = linear_free;
    linear->outside_methods.freeall = linear_freeall;
    linear->outside_methods.clone = linear_clone;

    Slice s = { linear, sizeof(LinearAllocator) };
    res.status = ERROR_OK;
    res.data = s;

    return res;
}

Result deinit_linear_allocator(LinearAllocator* linear) {
    Result res;
    BASE_ERROR_RESULT(res);

    res = FREEALL((Allocator*)linear);
    if (res.status != ERROR_OK) {
        return res;
    }

    res = FREE(linear->free_blocks.allocator, linear->free_blocks.buffer);
    if (res.status != ERROR_OK) {
        return res;
    }

    Slice s = { linear, sizeof(LinearAllocator) };
    res = FREE(linear->inside_methods, s);
    if (res.status != ERROR_OK) {
        return res;
    }

    res.status = ERROR_OK;
    res.data.length = 0;
    res.data.data = 0;

    return res;
}
